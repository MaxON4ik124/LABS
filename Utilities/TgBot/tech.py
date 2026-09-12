import telebot

API_TOKEN = "7203298205:AAF3saTT5Z4wqRsh6zdokbNd1nGA0EMxIS0"
bot = telebot.TeleBot(API_TOKEN)
@bot.message_handler(func=lambda message: True)
def echo_message(message):
    response = "Ведутся технические работы. Попробуйте позже."
    bot.reply_to(message, response)
bot.infinity_polling()