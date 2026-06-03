import base64
import json
import os
import threading

from flask import Flask, request, redirect, url_for, render_template_string
from kubernetes import client, config
from kubernetes.client.rest import ApiException
from werkzeug.security import generate_password_hash, check_password_hash


APP_NAME = os.getenv("APP_NAME", "flask-app")
SECRET_NAME = os.getenv("SECRET_NAME", "flask-users-secret")
SECRET_KEY = os.getenv("SECRET_KEY_NAME", "users.json")
NAMESPACE_FILE = "/var/run/secrets/kubernetes.io/serviceaccount/namespace"


def current_namespace() -> str:
    if os.path.exists(NAMESPACE_FILE):
        with open(NAMESPACE_FILE, "r", encoding="utf-8") as f:
            return f.read().strip()
    return os.getenv("POD_NAMESPACE", "app")


NAMESPACE = current_namespace()
write_lock = threading.Lock()

app = Flask(__name__)


def load_k8s_client():
    try:
        config.load_incluster_config()
    except config.ConfigException:
        config.load_kube_config()
    return client.CoreV1Api()


def read_users() -> dict:
    api = load_k8s_client()
    try:
        secret = api.read_namespaced_secret(SECRET_NAME, NAMESPACE)
    except ApiException as e:
        if e.status == 404:
            return {}
        raise

    if not secret.data or SECRET_KEY not in secret.data:
        return {}

    raw = base64.b64decode(secret.data[SECRET_KEY]).decode("utf-8")
    if not raw.strip():
        return {}

    return json.loads(raw)


def write_users(users: dict) -> None:
    api = load_k8s_client()
    encoded = base64.b64encode(
        json.dumps(users, ensure_ascii=False, indent=2).encode("utf-8")
    ).decode("utf-8")

    body = {
        "data": {
            SECRET_KEY: encoded
        }
    }

    api.patch_namespaced_secret(
        name=SECRET_NAME,
        namespace=NAMESPACE,
        body=body
    )


PAGE = """
<!doctype html>
<html lang="ru">
<head>
  <meta charset="utf-8">
  <title>{{ app_name }}</title>
  <style>
    body { font-family: Arial, sans-serif; max-width: 760px; margin: 40px auto; }
    form { border: 1px solid #ddd; padding: 16px; margin-bottom: 20px; border-radius: 8px; }
    input { display: block; margin: 8px 0 12px; padding: 8px; width: 280px; }
    button { padding: 8px 14px; cursor: pointer; }
    .msg { padding: 10px; background: #f3f3f3; border-radius: 6px; margin-bottom: 16px; }
    code { background: #eee; padding: 2px 5px; }
  </style>
</head>
<body>
  <h1>{{ app_name }}</h1>
  <p>Это учебное Flask-приложение. Пользователи хранятся в Kubernetes Secret <code>{{ secret_name }}</code>.</p>

  {% if message %}
    <div class="msg">{{ message }}</div>
  {% endif %}

  <form method="post" action="{{ base_path }}/register">
    <h2>Регистрация</h2>
    <label>Логин</label>
    <input name="username" required minlength="3">
    <label>Пароль</label>
    <input name="password" required minlength="4" type="password">
    <button type="submit">Зарегистрироваться</button>
  </form>

  <form method="post" action="{{ base_path }}/login">
    <h2>Вход</h2>
    <label>Логин</label>
    <input name="username" required>
    <label>Пароль</label>
    <input name="password" required type="password">
    <button type="submit">Войти</button>
  </form>
</body>
</html>
"""


def get_base_path() -> str:
    # Ingress переписывает путь /app1 -> /, но браузер должен отправлять формы обратно на /app1/...
    return request.headers.get("X-Forwarded-Prefix", "")


@app.route("/", methods=["GET"])
def index():
    return render_template_string(
        PAGE,
        app_name=APP_NAME,
        secret_name=SECRET_NAME,
        message=request.args.get("message"),
        base_path=get_base_path()
    )


@app.route("/register", methods=["POST"])
def register():
    username = request.form.get("username", "").strip()
    password = request.form.get("password", "")

    if not username or not password:
        return redirect(url_for("index", message="Логин и пароль обязательны"))

    with write_lock:
        users = read_users()
        if username in users:
            return redirect(url_for("index", message="Такой пользователь уже существует"))

        users[username] = {
            "password_hash": generate_password_hash(password),
            "created_by_app": APP_NAME
        }
        write_users(users)

    return redirect(url_for("index", message=f"Пользователь {username} зарегистрирован"))


@app.route("/login", methods=["POST"])
def login():
    username = request.form.get("username", "").strip()
    password = request.form.get("password", "")

    users = read_users()
    record = users.get(username)

    if not record:
        return redirect(url_for("index", message="Пользователь не найден"))

    if check_password_hash(record["password_hash"], password):
        return redirect(url_for("index", message=f"Успешный вход: {username}"))

    return redirect(url_for("index", message="Неверный пароль"))


@app.route("/healthz", methods=["GET"])
def healthz():
    return {"status": "ok", "app": APP_NAME}


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
