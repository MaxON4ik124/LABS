with open("output.txt", 'r') as f:
    T, N, Arr = f.readlines()
    Arr = list(map(int, Arr.split()))
    allright = 1
    for i in range(len(Arr)-1):
        cur = Arr[i]
        nex = Arr[i+1]
        if cur > nex:
            print(f"Mistake: {cur} {nex} {cur <= nex}")
            allright = 0
    if allright == 1:
        print("Correct!")