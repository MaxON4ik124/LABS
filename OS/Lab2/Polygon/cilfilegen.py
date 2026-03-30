import random
import time

def write_to_file(messages: list):
    with open("file.txt", "w") as f:
        for i in range(len(messages)):
            aa = random.randint(0, 65536)
            bbb = random.randint(-2147483648, 2147483648)
            curtime = time.ctime().split()[-2]
            f.write(f"{aa} {bbb} {curtime} {messages[i]}\n")
    f.close()


message_list = []
message = ''
while message != 'stop':
    message = input("Enter message (stop for exit): ")
    message_list.append(message)

write_to_file(message_list)
print("Messages successfully added to file")



