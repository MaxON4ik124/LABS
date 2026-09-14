def gcd(a,b) -> int:
    a1 = a
    b1 = b
    i = 1
    while b1 != 0:
        temp = a1
        a1 = b1
        b1 = temp % a1
        print(f"{i} iter: {a1}")
        i += 1
    return a1

print(gcd(9190812423861359177, 5665686725157642793))