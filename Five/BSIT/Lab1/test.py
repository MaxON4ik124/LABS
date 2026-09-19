data = "Secret"
key = "FCXSDF"
enc = [0] * 6
for i in range(len(data)):
    enc[i] = data[i] ^ key[i % len(key)]
print(enc)