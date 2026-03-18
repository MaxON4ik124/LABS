import requests
import burpsuite

main_response = requests.get("https://ruz.spbstu.ru")
print(main_response.text)
