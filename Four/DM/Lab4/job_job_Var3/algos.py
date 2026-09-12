from collections import deque
import math
f = open("job_Var3.in", "r")
n, var = map(int, f.readline().split())
G = [[0 for i in range(n)] for g in range(n)]
for i in range(n):
    G[i] = list(map(int, f.readline().split()))
# print(G)



def find_sources_sinks(C):
    n = len(C)
    sources, sinks = [], []
    
    for v in range(n):
        has_in = any(C[u][v] > 0 for u in range(n))
        has_out = any(C[v][u] > 0 for u in range(n))
        
        if not has_in:
            sources.append(v + 1)   # нумерация с 1
        if not has_out:
            sinks.append(v + 1)
            
    return sources, sinks


def edmonds_karp_with_logs(C):
    n = len(C)
    sources, sinks = find_sources_sinks(C)
    
    # --- создаём расширенную матрицу ---
    S = 0
    T = n + 1
    N = n + 2
    INF = 10**18
    
    capacity = [[0]*N for _ in range(N)]
    
    # копируем исходный граф (сдвиг на 1)
    for i in range(n):
        for j in range(n):
            capacity[i+1][j+1] = C[i][j]
    
    # супер-исток -> источники
    for s in sources:
        capacity[S][s] = INF
    
    # стоки -> супер-сток
    for t in sinks:
        capacity[t][T] = INF
    
    # матрица потоков
    flow = [[0]*N for _ in range(N)]
    
    iteration_flows = []
    max_flow = 0
    
    # --- BFS ---
    def bfs():
        parent = [-1]*N
        parent[S] = S
        q = deque([S])
        
        while q:
            v = q.popleft()
            for u in range(N):
                residual = capacity[v][u] - flow[v][u]
                if parent[u] == -1 and residual > 0:
                    parent[u] = v
                    q.append(u)
        return parent
    
    # --- основной цикл Edmonds-Karp ---
    while True:
        parent = bfs()
        if parent[T] == -1:
            break
        
        # ищем бутылочное горлышко
        add_flow = math.inf
        v = T
        while v != S:
            u = parent[v]
            add_flow = min(add_flow, capacity[u][v] - flow[u][v])
            v = u
        
        # проталкиваем поток
        v = T
        while v != S:
            u = parent[v]
            flow[u][v] += add_flow
            flow[v][u] -= add_flow
            v = u
        
        iteration_flows.append(add_flow)
        max_flow += add_flow
    
    # --- извлекаем итоговую матрицу потоков (без супер вершин) ---
    F = [[0]*n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            F[i][j] = max(flow[i+1][j+1], 0)
    
    return sources, sinks, iteration_flows, F, max_flow




def print_result(C):
    sources, sinks, iteration_flows, F, max_flow = edmonds_karp_with_logs(C)
    with open("job_Var3.out", "w") as f:
        f.write(" ".join(map(str, sources)) + '\n')
        f.write(" ".join(map(str, sinks)) + '\n')
        f.write(", ".join(map(str, iteration_flows)) + '\n')
        for row in F:
            f.write(" ".join(map(str, row)) + '\n')
        f.write(str(max_flow) + '\n')
print_result(G)