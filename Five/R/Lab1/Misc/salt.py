import random
key = list("0K0g0JzQntCX0JPQmCDQldCR0Jgg0J3QlSDQndCQ0JTQng==")

def is_prime(num):
    for i in range(2, num // 2 + 1):
        if num % i == 0:
            return False
    return True
salt = list("QWERTYUIOP[]ASDFGHJKL;'ZXCVBNM,./1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./!@#$%^&*()_+")
for i in range(len(key)):
    if is_prime(i):
        key.insert(i, random.choice(salt))

print("".join(key))

