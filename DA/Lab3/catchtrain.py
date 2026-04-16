import os
while True:
    curdir = os.listdir()
    if "DAmodel.pth" in curdir:
        os.system("./check.bat")
        break