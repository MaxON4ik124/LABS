import requests

r = requests.get("https://ruz.spbstu.ru/api/v1/ruz/buildings/")
print(r.text)