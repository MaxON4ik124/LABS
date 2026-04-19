def write_iter(i, filename, D, P, start):
    with open(filename, "a", encoding="utf-8") as f:
        f.write(f"{i}\n")
        out_D = []
        for idx, x in enumerate(D):
            if idx == start-1:
                out_D.append('*')
            elif x == 0:
                out_D.append('-')
            else:
                out_D.append(str(x))         
        f.write(f"D: {' '.join(out_D)}\n")
        f.write(f"P: {' '.join(map(str, P))}\n")


INF = 10**10

with open("job_Var15.in", "r") as f:
    n, var, start = map(int, f.readline().split())
    G = []
    for _ in range(n):
        row = [int(x) if x != '*' else '*' for x in f.readline().split()]
        G.append(row)

Visited = []
Unvisited = [x for x in range(n)]


D = [0] * n
P = [start for i in range(n)]

for i in range(n):
    if G[start-1][i] != '*':
        D[i] = G[start-1][i]
        P[i] = start

D[start - 1] = INF

iterr = 0
while len(Visited) != n-1:

    iterr += 1
    min_val = INF
    max_val = -1
    cur = -1

    for i in Unvisited:
        if D[i] > max_val:
            max_val = D[i]
            cur = i

    if cur == -1:
        break
    for i in Unvisited:
        if i != cur and G[cur][i] != '*':
            cand = min(D[cur], G[cur][i])
            if cand > D[i]:
                D[i] = cand
                P[i] = cur + 1
    write_iter(iterr, "job_Var15.out", D, P, start)

    Unvisited.remove(cur)
    Visited.append(cur)


