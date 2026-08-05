def f(x,y,z,t):
    return x ^ z ^ t ^ x*t ^ y*t ^ x*y ^ y*z ^ x*y*t

for x in range(2):
    for y in range(2):
        for z in range(2):
            for t in range(2):
                print(f"{x}{y}{z}{t} -> {f(x,y,z,t)}")