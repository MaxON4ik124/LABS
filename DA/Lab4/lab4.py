import logging
import asyncio
from typing import Dict, Optional
from flask import Flask, Response
from requests import request
import telebot
from telebot import TeleBot
import torch
from predict import predict_image


TOKEN = "8710564247:AAE7kg3b8YUt1aCgPUdEgPtjrl-eINlLYmI"
WEBHOOK_URL = "@da4CompVis_bot"
PORT = 8080

logging.basicConfig(
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s", level=logging.INFO
)
def register_handlers(bot: TeleBot) -> None:
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
        msg = bot.send_message(message.chat.id, "Введите пароль для нового пользователя")
        bot.register_next_step_handler(msg, register)
    
    def register(message):
        new_usr_id = list(user_storage.keys())[-1]+1 if user_storage != {} else 1
        user_storage[new_usr_id] = {"password": message.text, "authentificated" : False}
        bot.send_message(message.chat.id, "Пользователь зарегестрирован.")

    @bot.message_handler(commands=['login'])
    def handle_login(message):
        msg = bot.send_message(message.chat.id, "Введите пароль")
        bot.register_next_step_handler(msg, login)
    
    def login(message):
        found = 0
        for i in list(user_storage.keys()):
            if user_storage[i]["password"] == message.text:
                user_storage[i]["authentificated"] = True
                logged_users.setdefault(message.from_user.id, []).append(i)
                bot.send_message(message.chat.id, "Вы авторизованы")
                found = 1
                break
        if found == 0:
            bot.send_message(message.chat.id, "Пользователь не найден")

    @bot.message_handler(commands=['logout'])
    def handle_logout(message):
        if message.from_user.id in list(logged_users.keys()):
            logout_id = logged_users[message.from_user.id].pop()
            user_storage[logout_id]["authentificated"] = False
            bot.send_message(message.chat.id, "Вы успешно вышли")
        else:
            bot.send_message(message.chat.id, "Вы не авторизованы")

    @bot.message_handler(content_types=['photo'])
    def handle_predict(message):
        if user_storage == {}:
            bot.send_message(message.chat.id, "Вы не авторизованы")
        elif user_storage[logged_users[message.from_user.id][-1]]["authentificated"] == True:
            if message.caption != "/predict":
                bot.send_message(message.chat.id, "Отсутствует команда /predict")
            else:
                global global_image_bytes
                file_id = message.photo[-1].file_id
                file_info = bot.get_file(file_id)
                global_image_bytes = bot.download_file(file_info.file_path)
                Classes = ["Человек", "Собака"]
                pred, conf = predict_image(model, global_image_bytes)
                bot.send_message(message.chat.id, f"Распознанный тип: {Classes[pred]} {conf*100:.1f}%")
        else:
            bot.send_message(message.chat.id, "Вы не авторизованы")

    @bot.message_handler(content_types=['photo'])
    def handle_photo(message):
        if message.caption != "/predict":
            bot.send_message(message.chat.id, "Отсутствует команда /predict")

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

bot: Optional[TeleBot] = None

def get_bot() -> TeleBot:
    global bot
    if bot is None:
        bot = TeleBot(TOKEN)
        register_handlers(bot)
    return bot

global_image_bytes = None
logger = logging.getLogger(__name__)
user_storage: Dict[int, Dict[str, Optional[str | bool]]] = {}
model = torch.load("DAModel.pth", map_location="cpu", weights_only=False)
logged_users: Dict[int, list] = {}
flask_app = None
bot = get_bot()










bot.infinity_polling()

