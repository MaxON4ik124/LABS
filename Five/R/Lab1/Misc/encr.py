key = "!]$m0MK90g0GJ@zQn@t/CX0fJPQmCND[QldCR!0Jg]g&0J3BQlSDQndCQ0JTQng=="
keylen = len(key)

def is_prime(num):
    if num <= 3:
        return True

    for i in range(2, num):
        if num % i == 0:
            return False

    return True


with open("strings.txt", "r", encoding="utf-8") as file:
    for line in file:
        path = line.rstrip("\r\n")

        encoded = []
        key_index = 0

        for character in path:
            while is_prime(key_index % keylen):
                key_index += 1

            value = ord(character) ^ ord(key[key_index % keylen])
            encoded.append(f"\\x{value:02x}")
            key_index += 1

        print(f'{path} -> {"".join(encoded)}')