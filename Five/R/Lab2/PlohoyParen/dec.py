from itertools import product
Locale = [0x3F, 0x44, 0x45, 0x57, 0x3C, 0x29]
Key = 0x28
Offset = 0x10
Start = 0xFF750
Dest = 0xFF788
# print(Key + Offset)
entry = 'P'
ret = 's'
# print(bin(ord('P')).zfill(8), bin(ord('s')).zfill(8))
# print(Dest - Start)

# print(bin(0x23))
# print(chr(ord('P') ^ ord('s')))


SRCHASH=0x8BDEFF88
LEN=5
SUM=1612
HASH=0xC7722F1F
PASSLEN=547
inputpass = "sdfsdfsdsfd"
salt = "TripoSalt42!"

lowers = 'qwertyuiopasdfghjklzxcvbnm'

def hasher(password, salt=salt) -> int:
    if not isinstance(password, (str, bytes)):
        password = ''.join(password)
    password_bytes = password.encode('utf-8') if isinstance(password, str) else password
    salt_bytes = salt.encode('utf-8') if isinstance(salt, str) else salt

    hash_value = (-2128831035) & 0xFFFFFFFF
    for byte in salt_bytes:
        hash_value = (16777619 * (byte ^ hash_value)) & 0xFFFFFFFF
    hash_value = (16777619 * (hash_value ^ 0xA5)) & 0xFFFFFFFF
    for byte in password_bytes:
        hash_value = (16777619 * (byte ^ hash_value)) & 0xFFFFFFFF
    return hash_value

def chrsum(arr: list) -> int:
    return sum([ord(i) for i in arr])



for i in product(lowers, repeat=5):
    if(chrsum(i) == 547 and hasher(i) == SRCHASH):
        print(''.join(i))
