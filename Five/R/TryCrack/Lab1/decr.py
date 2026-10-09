def is_prime(n):
    """Check if a number is prime."""
    if n <= 1:
        return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            return False
    return True

PASS=[0x57, 0x20, 0x3, 0x1, 0x5c, 0x2e, 0x4a, 0x62, 0x9, 0x4f, 0x27, 0x00]
PASS = ''.join(chr(x) for x in PASS)
KEY=r"!]$m0MK90g0GJ@zQn@t/CX0fJPQmCND[QldCR!0Jg]g&0J3BQlSDQndCQ0JTQng=="

def encrypt(plain_text, key):
    """Encrypt the plain text using the provided key."""
    encrypted = []
    for i in range(len(plain_text)):
        if(not is_prime(i)):
            encrypted_char = chr(int(plain_text[i]) ^ ord(key[i % len(key)]))
            encrypted.append(encrypted_char)
    return ''.join(encrypted)

print(encrypt(PASS, KEY))