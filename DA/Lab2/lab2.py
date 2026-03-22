import requests
from bs4 import BeautifulSoup

teacher_id = 28961
date = "2026-3-16"
url = f"https://ruz.spbstu.ru/teachers/{teacher_id}?date={date}"

r = requests.get(url, headers={"User-Agent": "Mozilla/5.0"}, timeout=20)
r.raise_for_status()
r.encoding = "utf-8"

soup = BeautifulSoup(r.text, "html.parser")
print(soup.get_text("\n", strip=True))