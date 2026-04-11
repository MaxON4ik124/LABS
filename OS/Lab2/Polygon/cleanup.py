import os
# print(os.listdir())
for file in os.listdir():
    if file.split('.')[-1] == "stdout":
        os.remove(file)
    if file == 'msg.txt':
        os.remove("msg.txt")
os.remove("run.log")