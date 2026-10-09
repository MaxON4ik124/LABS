with open('data.txt', 'r') as file:
    for line in file:
        if(line.startswith("0x")):
            print(chr(int(line.strip(), 16)))