import random
N = 2*10**4
with open('input.txt', 'w') as f:
    f.write('32\n')
    f.write(f"{N}\n")
    for i in range(N):
        f.write(f"{random.randint(0,N)} ")