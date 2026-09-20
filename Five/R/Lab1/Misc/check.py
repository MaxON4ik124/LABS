with open("encrypted.txt", "rb") as f1, open("encsalt.txt", "rb") as f2:
    s1 = f1.readlines()
    s2 = f2.readlines()
    for i in range(len(s1)):
        if(s1 != s2): print("False")
