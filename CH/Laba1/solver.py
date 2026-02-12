words = ['bb4a', 'ef4f', '2d1d', 'fd41']
bins = []
for word in words:
    s = ""
    for l in list(word):
        s = s + bin(int(l, 16))[2:] + " "
    bins.append(s)
for binn in bins:
    print(binn)
    