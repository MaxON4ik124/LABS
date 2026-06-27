from __future__ import annotations

import json
import os
import re
import tempfile
import threading
from datetime import datetime, timezone
from functools import wraps
from pathlib import Path
from typing import Any
from uuid import uuid4

from lxml import etree
from flask import (
    Flask,
    jsonify,
    redirect,
    render_template,
    request,
    send_from_directory,
    session,
    url_for,
)
from werkzeug.security import check_password_hash, generate_password_hash


BASE_DIR = Path(__file__).resolve().parent
DATA_DIR = Path(os.getenv("DATA_DIR", BASE_DIR / "data"))
USERS_FILE = DATA_DIR / "users.json"
ORDERS_FILE = DATA_DIR / "orders.json"

DEFAULT_BALANCE = 10000
USERNAME_PATTERN = re.compile(r"^[a-zA-Zа-яА-ЯёЁ0-9_-]{3,30}$")

PRODUCTS: dict[str, dict[str, Any]] = {
    "tea": {
        "id": "tea",
        "name": "Монгольский чай",
        "description": "Традиционный солёный чай с молоком.",
        "price": 45,
        "icon": "🫖",
    },
    "boots": {
        "id": "boots",
        "name": "Походные сапоги",
        "description": "Тёплая обувь для долгих путешествий.",
        "price": 320,
        "icon": "🥾",
    },
    "map": {
        "id": "map",
        "name": "Карта степей",
        "description": "Подробная карта торговых путей.",
        "price": 90,
        "icon": "🗺️",
    },
    "tent": {
        "id": "tent",
        "name": "Дорожная юрта",
        "description": "Компактное укрытие для путешественника.",
        "price": 480,
        "icon": "⛺",
    },
    "bread": {
        "id": "bread",
        "name": "Степной хлеб",
        "description": "Свежая выпечка для дальней дороги.",
        "price": 25,
        "icon": "🥖",
    },
    "saddle": {
        "id": "saddle",
        "name": "Кожаное седло",
        "description": "Прочное седло ручной работы.",
        "price": 610,
        "icon": "🐎",
    },
}

storage_lock = threading.RLock()

app = Flask(__name__)
app.secret_key = os.getenv("FLASK_SECRET_KEY", "local-development-secret-change-me")
app.config.update(
    MAX_CONTENT_LENGTH=128 * 1024,
    SESSION_COOKIE_HTTPONLY=True,
    SESSION_COOKIE_SAMESITE="Lax",
)


def ensure_storage() -> None:
    DATA_DIR.mkdir(parents=True, exist_ok=True)

    if not USERS_FILE.exists():
        write_json(USERS_FILE, {})

    if not ORDERS_FILE.exists():
        write_json(ORDERS_FILE, {})


def read_json(path: Path) -> dict[str, Any]:
    with storage_lock:
        if not path.exists():
            return {}

        try:
            with path.open("r", encoding="utf-8") as file:
                value = json.load(file)
                return value if isinstance(value, dict) else {}
        except (json.JSONDecodeError, OSError):
            return {}


def write_json(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)

    with storage_lock:
        fd, temporary_name = tempfile.mkstemp(
            dir=path.parent,
            prefix=f".{path.name}.",
            suffix=".tmp",
        )

        try:
            with os.fdopen(fd, "w", encoding="utf-8") as file:
                json.dump(value, file, ensure_ascii=False, indent=2)
                file.flush()
                os.fsync(file.fileno())

            os.replace(temporary_name, path)
        except Exception:
            try:
                os.unlink(temporary_name)
            except OSError:
                pass
            raise


def normalize_username(username: str) -> str:
    return username.strip().lower()


def current_username() -> str | None:
    value = session.get("username")
    return value if isinstance(value, str) else None


def get_current_user() -> dict[str, Any] | None:
    username = current_username()

    if not username:
        return None

    return read_json(USERS_FILE).get(username)


def login_required_page(view):
    @wraps(view)
    def wrapped(*args, **kwargs):
        if not current_username():
            return redirect(url_for("login_page"))
        return view(*args, **kwargs)

    return wrapped


def login_required_api(view):
    @wraps(view)
    def wrapped(*args, **kwargs):
        if not current_username():
            return jsonify(
                success=False,
                error="Необходимо войти в аккаунт",
            ), 401
        return view(*args, **kwargs)

    return wrapped



@app.get("/styles/<path:filename>")
def styles_file(filename: str):
    return send_from_directory(BASE_DIR / "styles", filename)


@app.get("/js/<path:filename>")
def js_file(filename: str):
    return send_from_directory(BASE_DIR / "js", filename)


@app.context_processor
def inject_current_user():
    return {"current_user": get_current_user()}


@app.get("/")
def index():
    if current_username():
        return redirect(url_for("catalog_page"))
    return redirect(url_for("login_page"))


@app.get("/login")
def login_page():
    if current_username():
        return redirect(url_for("catalog_page"))
    return render_template("login.html")


@app.get("/register")
def register_page():
    if current_username():
        return redirect(url_for("catalog_page"))
    return render_template("register.html")


@app.get("/catalog")
@login_required_page
def catalog_page():
    return render_template("catalog.html", products=list(PRODUCTS.values()))


@app.get("/history")
@login_required_page
def history_page():
    username = current_username()
    all_orders = read_json(ORDERS_FILE)
    orders = all_orders.get(username, [])
    return render_template("history.html", orders=orders)


@app.get("/settings")
@login_required_page
def settings_page():
    return render_template("settings.html")


@app.post("/logout")
def logout():
    session.clear()
    return redirect(url_for("login_page"))


@app.post("/api/register")
def register_api():
    data = request.get_json(silent=True) or {}
    username = normalize_username(str(data.get("username", "")))
    password = str(data.get("password", ""))
    password_confirm = str(data.get("passwordConfirm", ""))

    if not USERNAME_PATTERN.fullmatch(username):
        return jsonify(
            success=False,
            error=(
                "Логин должен содержать от 3 до 30 букв, цифр, "
                "дефисов или символов подчёркивания"
            ),
        ), 400

    if len(password) < 6:
        return jsonify(
            success=False,
            error="Пароль должен содержать не менее 6 символов",
        ), 400

    if password != password_confirm:
        return jsonify(
            success=False,
            error="Пароли не совпадают",
        ), 400

    with storage_lock:
        users = read_json(USERS_FILE)

        if username in users:
            return jsonify(
                success=False,
                error="Пользователь с таким логином уже существует",
            ), 409

        users[username] = {
            "username": username,
            "password_hash": generate_password_hash(password),
            "balance": DEFAULT_BALANCE,
            "created_at": datetime.now(timezone.utc).isoformat(),
        }
        write_json(USERS_FILE, users)

    session["username"] = username

    return jsonify(
        success=True,
        redirect=url_for("catalog_page"),
    ), 201


@app.post("/api/login")
def login_api():
    data = request.get_json(silent=True) or {}
    username = normalize_username(str(data.get("username", "")))
    password = str(data.get("password", ""))

    user = read_json(USERS_FILE).get(username)

    if not user or not check_password_hash(user["password_hash"], password):
        return jsonify(
            success=False,
            error="Неверный логин или пароль",
        ), 401

    session["username"] = username

    return jsonify(
        success=True,
        redirect=url_for("catalog_page"),
    )


@app.get("/api/me")
@login_required_api
def me_api():
    user = get_current_user()

    return jsonify(
        success=True,
        user={
            "username": user["username"],
            "balance": user["balance"],
        },
    )


@app.post("/api/change-password")
@login_required_api
def change_password_api():
    data = request.get_json(silent=True) or {}
    current_password = str(data.get("currentPassword", ""))
    new_password = str(data.get("newPassword", ""))
    new_password_confirm = str(data.get("newPasswordConfirm", ""))
    username = current_username()

    if len(new_password) < 6:
        return jsonify(
            success=False,
            error="Новый пароль должен содержать не менее 6 символов",
        ), 400

    if new_password != new_password_confirm:
        return jsonify(
            success=False,
            error="Новые пароли не совпадают",
        ), 400

    if current_password == new_password:
        return jsonify(
            success=False,
            error="Новый пароль должен отличаться от текущего",
        ), 400

    with storage_lock:
        users = read_json(USERS_FILE)
        user = users.get(username)

        if not user or not check_password_hash(
            user["password_hash"],
            current_password,
        ):
            return jsonify(
                success=False,
                error="Текущий пароль введён неверно",
            ), 401

        user["password_hash"] = generate_password_hash(new_password)
        write_json(USERS_FILE, users)

    return jsonify(
        success=True,
        message="Пароль успешно изменён",
    )


@app.post("/api/purchase")
@login_required_api
def purchase_api():
    if request.mimetype not in {"application/xml", "text/xml"}:
        return jsonify(
            success=False,
            error="Ожидается Content-Type: application/xml",
        ), 415

    xml_bytes = request.get_data(cache=False)

    if not xml_bytes:
        return jsonify(
            success=False,
            error="XML-документ отсутствует",
        ), 400

    root = fromstring(
        xml_bytes,
        forbid_dtd=False,
        forbid_entities=False,
        forbid_external=False,
    )

    if root.tag != "order":
        return jsonify(
            success=False,
            error="Корневой элемент XML должен называться order",
        ), 422

    product_id = (root.findtext("product_id") or "").strip()
    quantity_text = (root.findtext("quantity") or "").strip()

    try:
        quantity = int(quantity_text)
    except ValueError:
        return jsonify(
            success=False,
            error="Количество должно быть целым числом",
        ), 422

    if quantity < 1 or quantity > 99:
        return jsonify(
            success=False,
            error="Количество должно находиться в диапазоне от 1 до 99",
        ), 422

    product = PRODUCTS.get(product_id)

    if not product:
        return jsonify(
            success=False,
            error="Товар не найден",
        ), 404

    total = product["price"] * quantity
    username = current_username()
    processed_at = datetime.now(timezone.utc).isoformat()

    with storage_lock:
        users = read_json(USERS_FILE)
        user = users.get(username)

        if not user:
            session.clear()
            return jsonify(
                success=False,
                error="Пользователь не найден",
            ), 401

        if user["balance"] < total:
            return jsonify(
                success=False,
                error="Недостаточно тугриков на балансе",
            ), 409

        user["balance"] -= total
        write_json(USERS_FILE, users)

        all_orders = read_json(ORDERS_FILE)
        user_orders = all_orders.setdefault(username, [])

        order = {
            "id": str(uuid4()),
            "created_at": processed_at,
            "product_id": product_id,
            "product_name": product["name"],
            "unit_price": product["price"],
            "quantity": quantity,
            "total": total,
        }

        user_orders.insert(0, order)
        write_json(ORDERS_FILE, all_orders)

    # Сервер возвращает клиенту результат обработки XML.
    xml_result = {
        "root_tag": root.tag,
        "product_id": product_id,
        "product_name": product["name"],
        "quantity": quantity,
        "unit_price": product["price"],
        "total": total,
        "processed_at": processed_at,
    }

    return jsonify(
        success=True,
        message="XML обработан, заказ создан",
        balance=user["balance"],
        order=order,
        xml_result=xml_result,
    ), 201


ensure_storage()

if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000, debug=True)
