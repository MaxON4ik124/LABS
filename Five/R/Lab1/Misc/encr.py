key = "0K0g0JzQntCX0JPQmCDQldCR0Jgg0J3QlSDQndCQ0JTQng=="

keylen = len(key)

def is_prime(num):
    if num <= 3:
        return True

    for i in range(2, num):
        if num % i == 0:
            return False

    return True
keyDES = []

with open("strings.txt", "r", encoding="utf-8") as file:
    for line in file:
        path = line.rstrip("\r\n")

        encoded = []
        key_index = 0

        for character in path:
            # while is_prime(key_index % keylen):
            #     key_index += 1
            
            value = ord(character) ^ ord(key[key_index % keylen])
            encoded.append(f"\\x{value:02x}")
            key_index += 1

        print(f'{path} -> {"".join(encoded)}')
key_index = 0
for k in key:
    while is_prime(key_index % keylen):
        key_index += 1
    keyDES.append(key[key_index % keylen])
    key_index += 1



print("".join(keyDES))