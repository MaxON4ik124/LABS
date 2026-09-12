f = open("map.txt", 'r')
X = []
Y = []
for y in range(1, 22):
    row = f.readline().split()
    # print(len(row))
    for x in range(1, len(row)+1):
        if row[x-1] == 'X':
            X.append((x, y))
        if(row[x-1] == 'Y'):
            Y.append((x, y))
X.sort(key=lambda x : x[0], reverse=True)
Y.sort(key=lambda x : x[0], reverse=True)
for i in Y:
    print("{" + f"{i[0]}" + ',' + f"{i[1]}" + '}')

