import random
import time

def write_to_file(file: str, message: str):
    with open(file, "a") as f:
        aa = random.randint(0, 65536)
        bbb = random.randint(-2147483648, 2147483648)
        curtime = time.ctime().split()[-2]
        f.write(f"{aa} {bbb} {curtime} {message}\n")





