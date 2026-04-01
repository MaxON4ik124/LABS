with open("data.txt", "r", encoding="utf-8") as f:
    data = f.read()
    print(len(data))
    for i in range(len(data)):
        print(ord(data[i]))