import zlib
from pathlib import Path


exe_path = Path("tanks.exe")
checksum_path = Path("Assets\\Sec\\checksum.txt")

crc = 0
with exe_path.open("rb") as file:
    while block := file.read(4096):
        crc = zlib.crc32(block, crc)

crc &= 0xFFFFFFFF
checksum_path.write_text(f"{crc:08X}\n", encoding="ascii")
print(f"{exe_path}: {crc:08X}")