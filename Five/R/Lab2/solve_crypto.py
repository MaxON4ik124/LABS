import itertools
import re
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple


HERE = Path(__file__).resolve().parent
ASM_PATH = HERE / "crypto.asm"
PASS_PATH = HERE / "pass.txt"


ASCII_MIN = 0x20
ASCII_MAX = 0x7E
PASSWORD_LENGTH = 0x31


def signed_char(value: int) -> int:
    value &= 0xFF
    return value - 0x100 if value & 0x80 else value


def c_div(a: int, b: int) -> int:
    q = abs(a) // abs(b)
    return q if (a < 0) == (b < 0) else -q


def c_rem(a: int, b: int) -> int:
    return a - c_div(a, b) * b


def normalize_line(line: str) -> str:
    line = re.sub(r"^\.text:[0-9A-Fa-f]+\s*", "", line)
    line = line.split(";", 1)[0]
    return re.sub(r"\s+", " ", line).strip()


def parse_number(token: str) -> int:
    token = token.strip()
    if token.lower().endswith("h"):
        return int(token[:-1], 16)
    return int(token, 10)


def read_targets(path: Path) -> List[int]:
    result: List[int] = []
    for line_no, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        token = raw.strip()
        if not token:
            continue
        try:
            result.append(int(token, 16))
        except ValueError as exc:
            raise ValueError(f"{path}:{line_no}: ожидалось hex-значение: {token!r}") from exc
    return result


def extract_blocks(asm_text: str) -> List[Tuple[str, List[str]]]:
    lines = asm_text.splitlines()
    starts: List[Tuple[int, str]] = []
    for i, raw in enumerate(lines):
        line = normalize_line(raw)
        match = re.search(r"\bxor dword_(976[0-9A-Fa-f]+)(?:\+[0-9])?, ebx$", line)
        if match:
            starts.append((i, match.group(1)))

    blocks: List[Tuple[str, List[str]]] = []
    for start_no, (start, target_name) in enumerate(starts):
        end = None
        for i in range(start + 1, len(lines)):
            line = normalize_line(lines[i])
            if re.match(r"cmp \w+, dword_976[0-9A-Fa-f]+$", line):
                end = i
                break
        block = [normalize_line(line) for line in lines[start + 1 : end + 1]]
        blocks.append((target_name, block))

    return blocks


def memory_value(operand: str, registers: Dict[str, int], password: Sequence[int]) -> int:
    match = re.fullmatch(
        r"\[(?:eax|ebx|ecx|edx|esi|edi)"
        r"(?:\+([0-9A-Fa-f]+)h|\+(\d+))?\]",
        operand,
    )
    offset = (
        int(match.group(1), 16)
        if match.group(1)
        else int(match.group(2), 10)
        if match.group(2)
        else 0
    )
    return signed_char(password[offset])


def execute_block(block: Sequence[str], password: Sequence[int]) -> int:
    regs: Dict[str, int] = {}

    for raw in block:
        line = raw
        if not line or line.endswith(":") or line.startswith("loc_"):
            continue
        if line.startswith(("j", "db ")):
            continue
        if "large fs:30h" in line or line in {"xor ebx, ebx", "mov bl, [eax+2]"}:
            continue
        if line.startswith("xor dword_"):
            continue

        match = re.fullmatch(r"mov (\w+), \[ebp-8\]", line)
        if match:
            regs[match.group(1)] = 0
            continue

        match = re.fullmatch(
            r"movsx (\w+), byte ptr (\[(?:eax|ebx|ecx|edx|esi|edi)"
            r"(?:\+[0-9A-Fa-f]+h|\+\d+)?\])",
            line,
        )
        if match:
            regs[match.group(1)] = memory_value(match.group(2), regs, password)
            continue

        match = re.fullmatch(r"mov (\w+), ([0-9A-Fa-f]+h|\d+)", line)
        if match:
            regs[match.group(1)] = parse_number(match.group(2))
            continue

        if line == "cdq":
            regs["edx"] = 0
            continue

        match = re.fullmatch(r"idiv (\w+)", line)
        if match:
            divisor = regs[match.group(1)]
            dividend = regs.get("eax", 0)
            regs["edx"] = c_rem(dividend, divisor)
            regs["eax"] = c_div(dividend, divisor)
            continue

        match = re.fullmatch(r"(add|sub|imul|xor) (\w+), (\w+|[0-9A-Fa-f]+h|\d+)", line)
        if match:
            operation, dst, src_token = match.groups()
            src = regs[src_token] if src_token in regs else parse_number(src_token)
            left = regs.get(dst, 0)
            if operation == "add":
                regs[dst] = left + src
            elif operation == "sub":
                regs[dst] = left - src
            elif operation == "imul":
                regs[dst] = left * src
            else:
                regs[dst] = left ^ src
            continue

        match = re.fullmatch(r"lea (\w+), \[(\w+)\+\2\*(\d+)\]", line)
        if match:
            dst, src, scale = match.groups()
            regs[dst] = regs.get(src, 0) * (1 + int(scale))
            continue

        match = re.fullmatch(r"lea (\w+), \[(\w+)\+(\w+)\*(\d+)\]", line)
        if match:
            dst, left, right, scale = match.groups()
            regs[dst] = regs.get(left, 0) + regs.get(right, 0) * int(scale)
            continue

        match = re.fullmatch(r"lea (\w+), ds:0\[(\w+)\*(\d+)\]", line)
        if match:
            dst, src, scale = match.groups()
            regs[dst] = regs.get(src, 0) * int(scale)
            continue

        match = re.fullmatch(r"cmp (\w+), dword_976[0-9A-Fa-f]+", line)
        if match:
            return regs.get(match.group(1), 0)


        if line.startswith(("inc ", "mov eax, large ")):
            continue


def all_values_match(
    password: Sequence[int],
    blocks: Sequence[Tuple[str, Sequence[str]]],
    targets: Sequence[int],
    start: int = 0,
) -> bool:
    for index in range(start, len(targets)):
        if execute_block(blocks[index][1], password) != targets[index]:
            return False
    return True


def simple_candidates(targets: Sequence[int]) -> List[List[Tuple[int, int, int, int]]]:
    groups: List[List[Tuple[int, int, int, int]]] = []
    for group in range(9):
        base = group * 4
        k = 0x13 + group
        target = group * 3
        candidates = []
        for first in range(ASCII_MIN, ASCII_MAX + 1):
            second = targets[target] - c_rem(first, k)
            third = targets[target + 1] - first - k
            fourth = targets[target + 2] - first * k
            if all(ASCII_MIN <= x <= ASCII_MAX for x in (second, third, fourth)):
                candidates.append((first, second, third, fourth))
        groups.append(candidates)

    return groups


def make_password(parts: Sequence[Tuple[int, ...]], targets: Sequence[int]) -> List[int]:
    password = [0] * PASSWORD_LENGTH
    for group, part in enumerate(parts[:9]):
        offset = group * 4
        password[offset : offset + 4] = part

    last = parts[9]
    password[36:40] = last[:4]
    password[40] = targets[30] - 28 * password[36]

    for offset, target_index in zip(range(41, 49), range(46, 54)):
        password[offset] = targets[target_index]

    return password


def solve(targets: Sequence[int], blocks: Sequence[Tuple[str, Sequence[str]]],) -> List[List[int]]:
    groups = simple_candidates(targets)

    last_group = []
    k = 0x1C
    for first in range(ASCII_MIN, ASCII_MAX + 1):
        p37 = targets[27] - c_rem(first, k)
        p38 = targets[28] - first - k
        p39 = targets[29] - first * k
        p40 = targets[30] - 28 * first
        if all(ASCII_MIN <= x <= ASCII_MAX for x in (p37, p38, p39, p40)):
            last_group.append((first, p37, p38, p39, p40))

    prefix_groups = groups
    solutions: List[List[int]] = []
    checked = 0

    for parts in itertools.product(*prefix_groups, last_group):
        password = make_password(parts, targets)
        checked += 1
        if not all(ASCII_MIN <= x <= ASCII_MAX for x in password):
            continue
        if all_values_match(password, blocks, targets, start=31):
            solutions.append(password)
    return solutions


def main() -> int:
    targets = read_targets(PASS_PATH)
    asm_text = ASM_PATH.read_text(encoding="utf-8")
    blocks = extract_blocks(asm_text)

    cmp_names = [re.search(r"cmp \w+, (dword_976[0-9A-Fa-f]+)", "\n".join(block)).group(1) for _, block in blocks]

    solutions = solve(targets, blocks)
    if not solutions:
        return 1

    for number, password in enumerate(solutions, 1):
        text = bytes(password).decode("ascii")
        print(f"[{number}] {text}")
        if len(text) == PASSWORD_LENGTH and all_values_match(password, blocks, targets):
            print("    verifier: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
