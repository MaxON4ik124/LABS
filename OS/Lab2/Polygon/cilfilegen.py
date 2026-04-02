import random
import time

def write_to_file(file: str, message: str):
    with open(file, "a") as f:
        aa = random.randint(0, 65536)
        bbb = random.randint(-2147483648, 2147483648)
        curtime = time.ctime().split()[-2]
        f.write(f"{aa} {bbb} {curtime} {message}\n")
    f.close()


# message_list = []
# message = ''
# while message != 'stop':
#     message = input("Enter message (stop for exit): ")
#     write_to_file(message)
#     message_list.append(message)

# print("Messages successfully added to file")



