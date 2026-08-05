from __future__ import annotations

import base64
import json
import os
import time
from pathlib import Path
from urllib.parse import urlparse
from urllib.request import Request, urlopen
from xml.sax.saxutils import escape as xml_escape

from flask import Flask, Response, jsonify, redirect, render_template, request, session, url_for
from lxml import etree
from sqlalchemy.exc import IntegrityError, OperationalError
from werkzeug.security import check_password_hash, generate_password_hash

from models import Order, Product, User, db


BASE_DIR = Path(__file__).resolve().parent
PRODUCTS_JSON = BASE_DIR / "data" / "products.json"

APP_VARIANT = os.getenv("APP_VARIANT", "v3").strip().lower()
if APP_VARIANT not in {"v1", "v2", "v3"}:
    raise RuntimeError("APP_VARIANT должен иметь значение v1, v2 или v3")

LAB_DB_EXPORT_ENABLED = (
    os.getenv("LAB_DB_EXPORT_ENABLED", "false").strip().lower()
    in {"1", "true", "yes", "on"}
)
MAX_REQUEST_BYTES = 128 * 1024
MAX_EXTERNAL_RESOURCE_BYTES = 1024 * 1024

app = Flask(__name__)
app.config["SECRET_KEY"] = os.getenv(
    "FLASK_SECRET_KEY",
    "local-development-key",
)
app.config["SQLALCHEMY_DATABASE_URI"] = os.getenv(
    "DATABASE_URL",
    "sqlite:///shop.db",
)
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
app.config["MAX_CONTENT_LENGTH"] = MAX_REQUEST_BYTES

db.init_app(app)


# ---------------------------------------------------------------------
# Инициализация базы данных
# ---------------------------------------------------------------------

def read_products_json() -> dict[str, dict]:
    if not PRODUCTS_JSON.exists():
        raise FileNotFoundError(f"Файл товаров не найден: {PRODUCTS_JSON}")

    with PRODUCTS_JSON.open("r", encoding="utf-8") as file:
        products_data = json.load(file)

    if not isinstance(products_data, dict):
        raise RuntimeError("Корневое значение products.json должно быть объектом")

    return products_data


def load_products_from_json() -> None:
    products_data = read_products_json()

    for product_id, data in products_data.items():
        if not isinstance(data, dict):
            raise RuntimeError(f"Товар {product_id} должен быть объектом")

        name = str(data.get("name", "")).strip()
        description = str(data.get("description", "")).strip()
        image_path = str(
            data.get("image_path", "img/no-image.png")
        ).strip()

        try:
            price = int(data["price"])
        except (KeyError, TypeError, ValueError) as error:
            raise RuntimeError(
                f"Некорректная цена товара {product_id}"
            ) from error

        if not name:
            raise RuntimeError(f"У товара {product_id} отсутствует название")

        if price < 0:
            raise RuntimeError(
                f"Цена товара {product_id} не может быть отрицательной"
            )

        product = db.session.get(Product, product_id)
        if product is None:
            product = Product(id=product_id)
            db.session.add(product)

        product.name = name
        product.description = description
        product.price = price
        product.image_path = image_path

    try:
        db.session.commit()
    except IntegrityError:

        db.session.rollback()
        for product_id, data in products_data.items():
            product = db.session.get(Product, product_id)
            if product is None:
                continue
            product.name = str(data["name"]).strip()
            product.description = str(data.get("description", "")).strip()
            product.price = int(data["price"])
            product.image_path = str(
                data.get("image_path", "img/no-image.png")
            ).strip()
        db.session.commit()


def initialize_database(attempts: int = 60, delay_seconds: float = 2.0) -> None:
    last_error: Exception | None = None

    for attempt in range(1, attempts + 1):
        try:
            with app.app_context():
                db.create_all()
                load_products_from_json()
            print("База данных готова", flush=True)
            return
        except (OperationalError, OSError) as error:
            last_error = error
            print(
                f"Ожидание базы данных: попытка {attempt}/{attempts}: {error}",
                flush=True,
            )
            time.sleep(delay_seconds)

    raise RuntimeError("Не удалось подключиться к базе данных") from last_error


# ---------------------------------------------------------------------
# Сессия пользователя
# ---------------------------------------------------------------------

def current_user() -> User | None:
    user_id = session.get("user_id")
    if user_id is None:
        return None
    return db.session.get(User, user_id)


@app.context_processor
def template_context() -> dict:
    account = current_user()
    return {
        "current_user": account,
        "current_balance": account.balance if account is not None else None,
    }


# ---------------------------------------------------------------------
# Разбор данных заказа
# ---------------------------------------------------------------------

class LabNetworkResolver(etree.Resolver):


    def resolve(self, system_url, public_id, context):
        parsed = urlparse(system_url)
        if parsed.scheme not in {"http", "https"}:
            return None

        request_object = Request(
            system_url,
            headers={"User-Agent": "xxe-demo-resolver/1.0"},
        )

        with urlopen(request_object, timeout=5) as response:
            content = response.read(MAX_EXTERNAL_RESOURCE_BYTES + 1)

        if len(content) > MAX_EXTERNAL_RESOURCE_BYTES:
            raise OSError("Внешний ресурс превышает допустимый размер")

        text = content.decode("utf-8", errors="strict")
        return self.resolve_string(text, context, base_url=system_url)


def create_order_parser() -> etree.XMLParser:
    if APP_VARIANT in {"v1", "v2"}:
        parser = etree.XMLParser(
            load_dtd=True,
            resolve_entities=True,
            no_network=False,
            huge_tree=True,
            recover=False,
        )
        parser.resolvers.add(LabNetworkResolver())
        return parser

    return etree.XMLParser(
        load_dtd=False,
        resolve_entities=False,
        no_network=True,
        huge_tree=False,
        recover=False,
    )

def parse_order_payload(payload: bytes):
    root = etree.fromstring(payload, create_order_parser())

    result = {}

    for child in root:
        key = child.tag
        value = (child.text or "").strip()

        result[key] = value

    return result
# ---------------------------------------------------------------------
# Локальный учебный экспорт данных БД
# ---------------------------------------------------------------------

def database_export_allowed() -> bool:
    return (
        APP_VARIANT in {"v1", "v2", "v3"}
        and LAB_DB_EXPORT_ENABLED
        and request.remote_addr in {"127.0.0.1", "::1"}
    )


def build_database_export() -> str:
    users = db.session.execute(
        db.select(
            User.id,
            User.username,
            User.password_hash,
            User.balance,
        ).order_by(User.id)
    ).mappings().all()

    orders = db.session.execute(
        db.select(
            Order.id,
            Order.user_id,
            Order.product_id,
            Order.product_name,
            Order.unit_price,
            Order.quantity,
            Order.total,
            Order.comment,
            Order.created_at,
        ).order_by(Order.id)
    ).mappings().all()

    payload = {
        "users": [dict(row) for row in users],
        "orders": [
            {
                **dict(row),
                "created_at": (
                    row["created_at"].isoformat()
                    if row["created_at"] is not None
                    else None
                ),
            }
            for row in orders
        ],
    }

    return json.dumps(
        payload,
        ensure_ascii=False,
        separators=(",", ":"),
    )


@app.get("/internal/lab/db-export")
def internal_db_export():
    if not database_export_allowed():
        return Response("Not found", status=404, content_type="text/plain")

    # Экранирование делает строку пригодной для подстановки как текст сущности.
    return Response(
        xml_escape(build_database_export()),
        content_type="text/plain; charset=utf-8",
    )


@app.get("/internal/lab/db-export-url")
def internal_db_export_url():
    if not database_export_allowed():
        return Response("Not found", status=404, content_type="text/plain")

    encoded = base64.urlsafe_b64encode(
        build_database_export().encode("utf-8")
    ).decode("ascii").rstrip("=")

    return Response(encoded, content_type="text/plain; charset=us-ascii")



@app.get("/health")
def health():
    try:
        db.session.execute(db.select(User.id).limit(1))
    except Exception:
        return jsonify(status="database unavailable"), 503
    return jsonify(status="ok", variant=APP_VARIANT)


@app.get("/")
def index():
    if current_user() is None:
        return redirect(url_for("login_page"))
    return redirect(url_for("shop_page"))


@app.get("/login")
def login_page():
    if current_user() is not None:
        return redirect(url_for("shop_page"))
    return render_template("login.html")


@app.get("/register")
def register_page():
    if current_user() is not None:
        return redirect(url_for("shop_page"))
    return render_template("register.html")


@app.get("/shop")
def shop_page():
    if current_user() is None:
        return redirect(url_for("login_page"))

    products = db.session.scalars(
        db.select(Product).order_by(Product.name)
    ).all()

    return render_template("shop.html", products=products)


@app.get("/orders")
def orders_page():
    account = current_user()
    if account is None:
        return redirect(url_for("login_page"))

    orders = db.session.scalars(
        db.select(Order)
        .where(Order.user_id == account.id)
        .order_by(Order.created_at.desc())
    ).all()

    return render_template("orders.html", orders=orders)


@app.post("/logout")
def logout():
    session.clear()
    return redirect(url_for("login_page"))


# ---------------------------------------------------------------------
# API пользователя
# ---------------------------------------------------------------------

@app.post("/api/register")
def register_api():
    data = request.get_json(silent=True) or {}
    username = str(data.get("username", "")).strip().lower()
    password = str(data.get("password", ""))

    if len(username) < 3:
        return jsonify(success=False, error="Логин слишком короткий"), 400
    if len(password) < 6:
        return jsonify(success=False, error="Пароль слишком короткий"), 400

    existing_user = db.session.scalar(
        db.select(User).where(User.username == username)
    )
    if existing_user is not None:
        return jsonify(
            success=False,
            error="Пользователь уже существует",
        ), 409

    account = User(
        username=username,
        password_hash=generate_password_hash(password),
        balance=1000,
    )

    try:
        db.session.add(account)
        db.session.commit()
    except IntegrityError:
        db.session.rollback()
        return jsonify(
            success=False,
            error="Пользователь уже существует",
        ), 409
    except Exception:
        db.session.rollback()
        return jsonify(
            success=False,
            error="Ошибка создания пользователя",
        ), 500

    session["user_id"] = account.id
    return jsonify(
        success=True,
        redirect=url_for("shop_page"),
    ), 201


@app.post("/api/login")
def login_api():
    data = request.get_json(silent=True) or {}
    username = str(data.get("username", "")).strip().lower()
    password = str(data.get("password", ""))

    account = db.session.scalar(
        db.select(User).where(User.username == username)
    )

    if account is None or not check_password_hash(
        account.password_hash,
        password,
    ):
        return jsonify(
            success=False,
            error="Неверный логин или пароль",
        ), 401

    session["user_id"] = account.id
    return jsonify(
        success=True,
        redirect=url_for("shop_page"),
    )


@app.post("/api/change-password")
def change_password_api():
    account = current_user()
    if account is None:
        return jsonify(success=False, error="Необходим вход"), 401

    data = request.get_json(silent=True) or {}
    current_password = str(data.get("currentPassword", ""))
    new_password = str(data.get("newPassword", ""))
    confirmation = str(data.get("newPasswordConfirm", ""))

    if not check_password_hash(account.password_hash, current_password):
        return jsonify(
            success=False,
            error="Неверный текущий пароль",
        ), 401
    if len(new_password) < 6:
        return jsonify(
            success=False,
            error="Новый пароль слишком короткий",
        ), 400
    if new_password != confirmation:
        return jsonify(success=False, error="Пароли не совпадают"), 400

    account.password_hash = generate_password_hash(new_password)

    try:
        db.session.commit()
    except Exception:
        db.session.rollback()
        return jsonify(
            success=False,
            error="Ошибка изменения пароля",
        ), 500

    return jsonify(success=True, message="Пароль изменён")


# ---------------------------------------------------------------------
# API покупки
# ---------------------------------------------------------------------

@app.post("/api/purchase")
def purchase_api():
    user_id = session.get("user_id")
    if user_id is None:
        return jsonify(success=False, error="Необходим вход"), 401

    try:
        parsed = parse_order_payload(request.get_data())

        product_id = parsed["product_id"]
        quantity = int(parsed["quantity"])
        comment = parsed.get("comment", "")
        raw_xml = parsed.get("raw")

    except (etree.XMLSyntaxError, UnicodeError, ValueError, OSError, KeyError) as error:
        app.logger.warning("PARSED DATA: %s", parsed)
        app.logger.warning("Order processing failed: %s", error)
        return jsonify(
            success=False,
            error="Error processing order",
        ), 400

    try:
        account = db.session.scalar(
            db.select(User)
            .where(User.id == user_id)
            .with_for_update()
        )
        product = db.session.get(Product, product_id)

        if account is None:
            db.session.rollback()
            return jsonify(success=False, error="User not found"), 404

        if product is None:
            db.session.rollback()
            return jsonify(success=False, error="Product not found"), 404

        total = product.price * quantity

        if account.balance < total:
            db.session.rollback()
            return jsonify(
                success=False,
                error="Insufficient funds",
                balance=account.balance,
            ), 409

        account.balance -= total

        order = Order(
            user_id=account.id,
            product_id=product.id,
            product_name=product.name,
            unit_price=product.price,
            quantity=quantity,
            total=total,
            comment=comment,
        )

        db.session.add(order)
        db.session.commit()

    except Exception:
        db.session.rollback()
        app.logger.exception("Purchase transaction failed")
        return jsonify(
            success=False,
            error="Error processing purchase",
        ), 500


    if APP_VARIANT == "v1":
        return jsonify({
            "success": True,
            "message": "Purchase successfully processed",
            "balance": account.balance,
            "comment": comment,
            "xml": raw_xml,
            "processing_result": parsed,
            "order_id": order.id,
        }), 201

    return jsonify({
        "success": True,
        "message": "Purchase successfully processed",
        "balance": account.balance,
    }), 201


if __name__ == "__main__":
    print(f"APP_VARIANT={APP_VARIANT}", flush=True)
    print(f"DATABASE={app.config['SQLALCHEMY_DATABASE_URI']}", flush=True)
    print(f"LAB_DB_EXPORT_ENABLED={LAB_DB_EXPORT_ENABLED}", flush=True)

    initialize_database()

    app.run(
        host="0.0.0.0",
        port=5000,
        debug=False,
        threaded=True,
    )
