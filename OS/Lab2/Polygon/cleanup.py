import os
# print(os.listdir())
for file in os.listdir():
    if file.split('.')[-1] == "stdout":
        os.remove(file)
    if 'msg.txt' in file:
        os.remove("msg.txt")
    if "cli" and ".txt" in file:
        os.remove(file)
    if "run.log" in file:
        os.remove("run.log")