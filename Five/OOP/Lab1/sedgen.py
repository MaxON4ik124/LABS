from pathlib import Path

out = Path("input")

for op in range(15):
    data = bytes([op, 0, 1, 0, 1]) + b"abc"
    (out / f"op_{op}").write_bytes(data)
cases = [
    bytes([0, 0, 0, 0, 0]) + b"",
    bytes([1, 1, 255, 2, 255]) + b"abc",
    bytes([4, 255, 255, 0, 0]) + b"abc",
    bytes([5, 1, 0, 0, 255]) + b"abcdef",
    bytes([7, 2, 3, 0, 0]) + b"abcdef",
    bytes([11, 0, 0, 0, 0]) + b"abc",
    bytes([12, 0, 0, 0, 0]) + b"123",
    bytes([13, 0, 0, 0, 0]) + b"1.25",
    bytes([14, 0, 0, 0, 0]) + b"abc",
]

for i, data in enumerate(cases):
    (out / f"edge_{i}").write_bytes(data)