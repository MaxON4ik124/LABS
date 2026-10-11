from itertools import product
from math import prod
encr = 0
pattern = 0




with open("data.txt", "r") as f:
    for line in f:
        line = line.strip("\r\n")
        if line.startswith("right="):
            encr = line[len("right="):]
        if line.startswith("pattern="):
            pattern = line[len("pattern="):]


encr, pattern = list(encr), [ord(i) for i in list(pattern)]

v7 = ["65 51 8D 53","18 98 52 72","6E A7 65 A4","64 9D 62 61","14 72 36 37","6D 73 44 08","2E 64 5D 37","34 63 07 31","17 18 1B 1A","00 00 48 38"]
v7 = [list(reversed(i.split())) for i in v7]
v7_1 = [(int(j, 16)) for i in v7 for j in i]
for i in range(50):
    idx = ord(encr[i]) - 48
    op = pattern[i]
    if op == 48:
        v7_1[idx] += 1
    if op == 49:
        v7_1[idx] ^= 0x2F
    if op == 50:
        v7_1[idx] -= 51
    if op == 51:
        v7_1[idx] *= 2
print(v7_1)
print("".join(list(map(chr, v7_1))))

print([ord(i) - 48 for i in encr])


            


