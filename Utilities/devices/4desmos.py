f = open('f.txt').read().split()
U = open('C3.txt').read().split()
r = open('res.txt', 'w')
for i in range(11):
    U[i] = U[i].replace(',', '.', 1)
    print(f"{f[i]},{U[i]},")
    r.write(f"{f[i]},{U[i]},\n")