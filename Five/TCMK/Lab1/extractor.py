import re
from docx import Document
G = []
X = []
Y = []
Iters = []
with open('out.txt', encoding="utf-8") as f:
    G_i = []
    X_i = []
    Y_i = []
    I_i = []
    tableNum = 0
    
    for line in f:
        if("итерация" in line):
            line = re.split(r"[=, \n]", line)
            if(int(line[0]) == 1 and tableNum != 0):
                if(len(G_i) > 20):
                    G_temp = G_i[:5]
                    G_temp.extend(G_i[-5:])
                    X_temp = X_i[:5]
                    X_temp.extend(X_i[-5:])
                    Y_temp = Y_i[:5]
                    Y_temp.extend(Y_i[-5:])
                    I_temp = I_i[:5]
                    I_temp.extend(I_i[-5:])
                else:
                    G_temp = G_i
                    X_temp = X_i
                    Y_temp = Y_i
                    I_temp = I_i
                G.append(G_temp)
                X.append(X_temp)
                Y.append(Y_temp)
                Iters.append(I_temp)
                G_i = []
                X_i = []
                Y_i = []
                I_i = []
                tableNum += 1
            if(tableNum == 0): tableNum = 1
            I_i.append(line[0])
            G_i.append(line[3])
            X_i.append(line[6])
            Y_i.append(line[9])

doc = Document()
for i in range(tableNum-1):
    table = doc.add_table(rows=1, cols=4)
    headers = table.rows[0].cells
    headers[0].text = "Шаг"
    headers[1].text = "Остаток"
    headers[2].text = "x"
    headers[3].text = "y"
    for r in range(len(Iters[i])):
        cells = table.add_row().cells
        cells[0].text = Iters[i][r]
        cells[1].text = G[i][r]
        cells[2].text = X[i][r]
        cells[3].text = Y[i][r]

doc.save("tables.docx")

