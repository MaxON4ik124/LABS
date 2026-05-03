import random
N = 1*10**5
with open('input.txt', 'w') as f:
    f.write('8\n')
    f.write(f"{N}\n")
    for i in range(N):
        f.write(f"{random.randint(0,N)} ")