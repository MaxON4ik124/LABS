import requests
import re
import json
url_pasre4teacher = f"https://ruz.spbstu.ru/search/teacher?q="
url_parse4group = f"https://ruz.spbstu.ru/search/groups?q="
def print_subject_info(subject):
    subject_title = ""
    subject_title = subject_title + f"Time:{subject.get("time_start")}-{subject.get("time_end")}\n"
    subject_title = subject_title + f"Subject:{subject.get("subject")}\n"
    subject_title = subject_title + f"Type:{subject.get("typeObj", {}).get("name")}\n"
    if subject.get("teachers", [{}]): subject_title = subject_title + f"Teacher:{subject.get("teachers", [{}])[0].get("full_name")}\n"
    else: subject_title = subject_title + "Teacher:None\n"
    subject_title = subject_title + f"Groups:{','.join([group.get("name") for group in subject.get("groups", [])])}\n"
    subject_title = subject_title + f"Place:{subject.get("auditories", [{}])[0].get("name")}\n"
    return subject_title

def get_teacher_shedule(teacher: str, date: str):
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

def get_group_shedule(group: str, date: str):
    shedule = ''
    group_id = None
    found = False
    group4req = group.replace("/", "%2F")
    url = url_parse4group + group4req
    r = requests.get(url)
    data = r.text
    group_t = [x for x in re.findall(r"<a.*?>.*?</a>", data) if group in x][0]
    href = group_t.split()[2]
    group_id = re.split('/|"', href)[-2]
    api_req_2 = requests.get(f"https://ruz.spbstu.ru/api/v1/ruz/scheduler/{group_id}")
    data = api_req_2.json()
    days = data["days"]
    for day in days:
        if day["date"] == date:
            for lesson in day.get("lessons"):
                found = True
                shedule = shedule + print_subject_info(lesson)
    if not found: return None
    return shedule

def get_room_shedule(building: str, room: str, date):
    shedule = ''
    found = False
    buildings = requests.get("https://ruz.spbstu.ru/api/v1/ruz/buildings/").json()
    build_id, room_id = 0, 0
    for build in buildings.get("buildings"):
        if building == build.get("name"):
            build_id = build.get("id")
            break
    rooms = requests.get(f"https://ruz.spbstu.ru/api/v1/ruz/buildings/{build_id}/rooms/").json()
    for aud in rooms.get("rooms"):
        if room == aud.get("name"):
            room_id = aud.get("id")
            break
    api_req_3 = requests.get(f"https://ruz.spbstu.ru/api/v1/ruz/buildings/{build_id}/rooms/{room_id}/scheduler?date={date}")
    data = api_req_3.json()
    with open("data_a.json", "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=4)
    days = data["days"]

    for day in days:
        if day["date"] == date:
            for lesson in day.get("lessons"):
                found = True
                shedule = shedule + print_subject_info(lesson)
    if not found: return None
    return shedule


# print(get_teacher_shedule("Макаров Александр Сергеевич", "2026-03-27"))
# print(get_group_shedule("5151003/40002", "2026-03-25"))
# print(get_room_shedule("Главное здание", '237', "2026-03-23"))


