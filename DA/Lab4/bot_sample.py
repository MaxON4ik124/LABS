import logging
import asyncio
from typing import Dict, Optional
from flask import Flask, Response

import telebot
from telebot import TeleBot

from predict import predict_image

TOKEN = ""
WEBHOOK_URL = ""
PORT = 8080

logging.basicConfig(
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s", level=logging.INFO
)
logger = logging.getLogger(__name__)

user_storage: Dict[int, Dict[str, Optional[str | bool]]] = {}
model = None

bot: Optional[TeleBot] = None
flask_app = None


def get_bot() -> TeleBot:
    global bot
    if bot is None:
        bot = TeleBot(TOKEN)
        _register_handlers(bot)
    return bot


def _register_handlers(bot: TeleBot) -> None:
    @bot.message_handler(commands=['start', 'help'])
    def handle_start(message):
        help_text = """
*Доступные команды:*

/register <пароль> - регистрация нового пользователя
/login <пароль>    - вход в систему
/logout            - завершение сессии
/predict           - классифицировать изображение (человек/животное)

*Пример использования:*
1. /register mysecretpass
2. /login mysecretpass
3. Отправьте фото с подписью /predict
        """
        bot.send_message(message.chat.id, help_text, parse_mode="Markdown")

    @bot.message_handler(commands=['register'])
    def handle_register(message):
        pass

    @bot.message_handler(commands=['login'])
    def handle_login(message):
        pass

    @bot.message_handler(commands=['logout'])
    def handle_logout(message):
        pass

    @bot.message_handler(commands=['predict'], content_types=['photo'])
    def handle_predict(message):
        pass

    @bot.message_handler(content_types=['photo'])
    def handle_photo(message):
        pass


def get_flask_app() -> Flask:
    global flask_app
    if flask_app is None:
        flask_app = Flask(__name__)
        _setup_routes(flask_app)
    return flask_app


def _setup_routes(app: Flask) -> None:
    if app is None:
        return
    
    @app.route(f"/{TOKEN}", methods=['POST'])
    def telegram_webhook():
        if not bot:
            return Response("Bot not initialized", status=500)
        
        json_data = request.json
        if json_data:
            bot.process_new_updates([telebot.types.Update.de_json(json_data)])
        return Response("OK")
    
    @app.route("/health", methods=['GET'])
    def healthcheck():
        return Response("OK")

