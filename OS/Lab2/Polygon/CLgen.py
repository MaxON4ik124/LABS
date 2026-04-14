import random
import time


def write_to_file(file: str, message: str):
    with open(file, "a") as f:
        aa = random.randint(0, 65536)
        bbb = random.randint(-2147483648, 2147483648)
        curtime = time.ctime().split()[-2]
        f.write(f"{aa} {bbb} {curtime} {message}\n")





n = 1
# 1 тест TCP клиента
for g in range(1, 3):
    for k in range(1, 3):
        write_to_file(f"1cli{g}.txt", f"msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n} msg {n}")
        n += 1
write_to_file("1cli3.txt", "stop")
# 2 тест UDP клиента
for i in range(1, 11):
    for g in range(1, 21):
        write_to_file(f"2cli{i}.txt", f"{i}___msg{g}.")
write_to_file("2cli11.txt", "stop")
# 3 теста UDP сервера
for i in range(1, 4):
    for g in range(1, 41):
        for k in range(1, 21):
            write_to_file(f"{i+2}cli{g}.txt", f"cli{g}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.msg{k}.")
    write_to_file(f"{i+2}cli41.txt", "stop")