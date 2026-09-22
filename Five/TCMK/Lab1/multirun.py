import subprocess
import tqdm
Times = [0] * 9

runs = 300

with open("output.txt", "r+") as f:
    for i in tqdm.tqdm(range(runs)):
        f.seek(0)
        f.truncate()

        res = subprocess.run(
            ["java", "C:\\Users\\Maks\\Desktop\\Labs\\Five\\TCMK\\Lab1\\algos.java"],
            stdout=f,
            stderr=subprocess.STDOUT,
            text=True,
            check=True
        )

        f.seek(0)
        times = f.readlines()
        times = [i.strip() for i in times]
        for j in range(9):
            # print(j)
            Times[j] += float(times[j].replace(",", "."))
print([i / runs * 1000 for i in Times])