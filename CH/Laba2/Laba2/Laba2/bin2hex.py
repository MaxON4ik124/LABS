a = input()
while a != 'stop' :
    if a == "bin":
        a = input()
        tmp = int(a, 2)
        print(hex(tmp))
    elif a == "hex":
        a = input()
        tmp = int(a, 16)
        print(bin(tmp))
    a = input()