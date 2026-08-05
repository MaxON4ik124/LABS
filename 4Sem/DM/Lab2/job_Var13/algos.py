def write_iter(i, filename, D, P):
    with open(filename, "a", encoding="utf-8") as f:
        f.write(f"{i}\n")
        f.write(f"D: {' '.join('*' if x == INF else str(x) for x in D)}\n")
        f.write(f"P: {' '.join(map(str, P))}\n")


INF = 10**10

with open("job_Var13.in", "r") as f:
    n, var, start = map(int, f.readline().split())
    G = []
    for _ in range(n):
        row = [int(x) if x != '*' else '*' for x in f.readline().split()]
        G.append(row)

Visited = []
Unvisited = [x for x in range(n)]


D = [INF] * n
P = [start for i in range(n)]

for i in range(n):
    if G[start-1][i] != '*':
        D[i] = G[start-1][i]
        P[i] = start

D[start - 1] = 0
P[start - 1] = start

iterr = 0
while len(Visited) != n-1:

    iterr += 1
    min_val = INF
    cur = -1

    for i in Unvisited:
        if D[i] < min_val:
            min_val = D[i]
            cur = i

    if cur == -1:
        break
    for i in Unvisited:
        if i != cur and G[cur][i] != '*':
            if D[cur] + G[cur][i] < D[i]:
                D[i] = D[cur] + G[cur][i]
                P[i] = cur + 1
    write_iter(iterr, "job_Var13.out", D, P)

    Unvisited.remove(cur)
    Visited.append(cur)


