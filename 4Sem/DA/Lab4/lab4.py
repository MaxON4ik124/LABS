import logging
from typing import Dict, Optional

from flask import Flask, Response, request
import telebot
from telebot import TeleBot
import torch

from predict import predict_image
#1
TOKEN = "8710564247:AAE7kg3b8YUt1aCgPUdEgPtjrl-eINlLYmI"
WEBHOOK_URL = "https://skeleton-heaving-hydrogen.ngrok-free.dev"
PORT = 8081

logging.basicConfig(
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
    level=logging.INFO
)

logger = logging.getLogger(__name__)

bot: Optional[TeleBot] = None
flask_app = None

global_image_bytes = None

user_storage: Dict[int, Dict[str, Optional[str | bool]]] = {}


model = torch.load("DAModel.pth", map_location="cpu", weights_only=False)


def register_handlers(bot: TeleBot) -> None:

    @bot.message_handler(commands=["start", "help"])
    def handle_start(message):
        help_text = """
*Доступные команды:*

/register - регистрация нового пользователя
/login    - вход в систему
/logout   - завершение сессии

Для распознавания отправьте фото с подписью:
/predict
"""
        bot.send_message(message.chat.id, help_text, parse_mode="Markdown")

    @bot.message_handler(commands=["register"])
    def handle_register(message):
        msg = message.text.split()[-1]
        if msg in [i["password"] for i in list(user_storage.values())]:
            bot.send_message(message.chat.id, "Данный пользователь уже существует")
        else:
            new_usr_id = message.from_user.id
            user_storage[new_usr_id] = {"password": msg, "authenticated": False}
            bot.send_message(message.chat.id, "Пользователь зарегистрирован.")

    @bot.message_handler(commands=["login"])A
    def handle_login(message):
        msg = message.text.split()[-1]
        if message.from_user.id in user_storage and user_storage[message.from_user.id]["password"] == msg:
            user_storage[message.from_user.id]["authenticated"] = True
            bot.send_message(message.chat.id, "Вы авторизованы")
        else:
            bot.send_message(message.chat.id, "Пользователь не найден")
    
    @bot.message_handler(commands=["logout"])
    def handle_logout(message):
        if message.from_user.id in user_storage:
            user_storage[message.from_user.id]["authenticated"] = False
            bot.send_message(message.chat.id, "Вы успешно вышли")
        else:
            bot.send_message(message.chat.id, "Вы не авторизованы")

    @bot.message_handler(content_types=["photo"], commands=["predict"])
    def handle_predict(message):
        try:
            telegram_user_id = message.from_user.id
            if telegram_user_id not in user_storage or user_storage[telegram_user_id]["authenticated"] == False:
                bot.send_message(message.chat.id, "Вы не авторизованы")
            else:
                if not getattr(message, "photo", None):
                    bot.send_message(message.chat.id, "Отсутствует изображение")
                    return
                file_id = message.photo[-1].file_id
                file_info = bot.get_file(file_id)
                image_bytes = bot.download_file(file_info.file_path)
                classes = ["Человек", "Собака"]
                pred, conf = predict_image(model, image_bytes)
                bot.send_message(message.chat.id, f"Распознанный тип: {classes[pred]} {conf * 100:.1f}%")
        except Exception:
            bot.send_message(message.chat.id, "Не удалось обработать изображение")
    @bot.message_handler(content_types=["photo"])
    def handle_photo(message):
        bot.send_message(message.chat.id, "Недействительный формат команды")

def get_bot() -> TeleBot:
    global bot

    if bot is None:
        bot = TeleBot(TOKEN)
        register_handlers(bot)

    return bot


def get_flask_app() -> Flask:
    global flask_app

    if flask_app is None:
        flask_app = Flask(__name__)
        _setup_routes(flask_app)
    return flask_app

def _setup_routes(app: Flask) -> None:

    @app.route(f"/{TOKEN}", methods=["POST"])
    def telegram_webhook():
        current_bot = get_bot()

        json_data = request.get_json()

        if json_data:
            update = telebot.types.Update.de_json(json_data)
            current_bot.process_new_updates([update])

        return Response("OK", status=200)

    @app.route("/health", methods=["GET"])
    def healthcheck():
        return Response("OK", status=200)


if __name__ == "__main__":
    bot = get_bot()
    bot.remove_webhook()
    bot.set_webhook(url=f"{WEBHOOK_URL}/{TOKEN}")
    flask_app = get_flask_app()
    flask_app.run(host="0.0.0.0",port=PORT)

    # bot.infinity_polling()