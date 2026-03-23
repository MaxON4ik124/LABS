import requests
import re
import json

teacher = 'Мизина Виктория Валерьевна'
date = '2026-03-16'
main_url = "https://ruz.spbstu.ru"
url_pasre4teacher = f"https://ruz.spbstu.ru/search/teacher?q="
url_parse4group = f"https://ruz.spbstu.ru/search/groups?q="
def print_subject_info(subject):
    subject_title = ""
    subject_title = subject_title + f"Time:{subject.get("time_start")}-{subject.get("time_end")}\n"
    subject_title = subject_title + f"Subject:{subject.get("subject")}\n"
    subject_title = subject_title + f"Type:{subject.get("typeObj", {}).get("name")}\n"
    subject_title = subject_title + f"Teacher:{subject.get("teachers", [{}])[0].get("full_name")}\n"
    subject_title = subject_title + f"Groups:{','.join([group.get("name") for group in subject.get("groups", [])])}\n"
    subject_title = subject_title + f"Place:{subject.get("auditories", [{}])[0].get("name")}\n"
    return subject_title

def get_teacher_shedule(teacher, date):
    shedule = ''
    teacher_id = None
    found = False
    url = url_pasre4teacher + teacher
    r = requests.get(url)
    data = r.text
    teachers = re.findall(r"<a.*?>.*?</a>", data)
    for t in teachers:
        if teacher in t:
            teacher_id_raw = (re.findall(r"<a(.*?)>.*?</a>", data)[teachers.index(t)].split())[-2]
            teacher_id = (re.split('"|/', teacher_id_raw))[-2]
            break         
    api_req_1 = requests.get(f"https://ruz.spbstu.ru/api/v1/ruz/teachers/{int(teacher_id)}/scheduler?date={date}")
    data = api_req_1.json()
    days = data["days"]
    for day in days:
        if day["date"] == date:
            for lesson in day.get("lessons"):
                found = True
                shedule = shedule + print_subject_info(lesson)
    if not found: return None
    return shedule




# print(get_teacher_shedule("Истомина Анастасия Сергеевна", "2026-03-16"))

