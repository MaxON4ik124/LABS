import random
from collections import defaultdict

N = 32
MIN_VALUE = 1
MAX_VALUE = 25
THREADS = 12


A = [random.randint(MIN_VALUE, MAX_VALUE) for _ in range(N)]

signs = [1]
for _ in range(1, N):
    signs.append(random.choice([1, -1]))

SUM = sum(A[i] * signs[i] for i in range(N))

dp = defaultdict(int)
dp[A[0]] = 1

for i in range(1, N):
    new_dp = defaultdict(int)

    for current_sum, count in dp.items():
        new_dp[current_sum + A[i]] += count
        new_dp[current_sum - A[i]] += count

    dp = new_dp

answer = dp[SUM]

with open("input.txt", "w") as file:
    file.write(f"{THREADS}\n{N}\n")
    for x in A:
        file.write(str(x) + " ")
    file.write(str(SUM))
    print(answer)