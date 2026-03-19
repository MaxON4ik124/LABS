import requests

main_response = requests.get("https://ruz.spbstu.ru")
print(main_response.text)
