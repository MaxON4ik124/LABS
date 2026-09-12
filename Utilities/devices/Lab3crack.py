import math
from docx import Document
from openpyxl import *
wd = Workbook()
wl = wd.active
doc = Document()
C = 10**-8
L = 5*10**-3
Vs = [1, 5, 10, 20, 30, 40, 50, 60, 70, 80]
def write_subscript(cell, text):
    run = cell.paragraphs[0].add_run(text)
    run.font.subscript = True

def calc_v_fhf_С(c, f):
    Xc = 1/(2*math.pi*f*c)
    R = 1000 + Xc
    I = 10/R
    return 1000*I * (2**0.5)
def calc_v_flf_С(c, f):
    w = 2*math.pi*f
    k = 1/(1+(w*1000*c)**2)**0.5
    return 10*k
def calc_v_fhf_L(l, f):
    Rl = 2*math.pi*l*f
    Vo = 10*(Rl/1000)/(1 + (Rl/1000)**2)**0.5
    return Vo
def calc_v_fhf_L(l, f):
    Rl = 2*math.pi*l*f
    Vo = 10/(1 + (Rl/1000)**2)**0.5
    return Vo
def calc_v_fhf_T(c, f):
    x = 1000 * 2 * math.pi * f * c
    P1 = (x / math.sqrt(1 + x**2)) * ((1 + 1j/x) / (2 + 1j/x))
    P2 = (1 + 1j/x) * (1 + 1/(2 + 1j/x))
    return (10 * abs(P1/P2) * math.sqrt(2)) / math.sqrt(2)
def calc_v_flf_T(c, f):
    w = 2 * math.pi * f
    Zc = 1 / (1j * w * c)
    R = 1000
    Z = (R * Zc) / (R + Zc)
    V1 = 10 * (Z / (R + Z))
    return abs(V1 * 0.5)
def calc_v_fhf_P(c, f):
    w = 2 * math.pi * f
    x = 1000 * c * w
    H = x/(4*(x + 1))
    return 10 * H
def calc_v_flf_P(c, f):
    w = 2 * math.pi * f
    x = 1000 * c * w
    H = 1/(2 * (x + 1))
    return 10 * H
def calc_v_rez_P(c, l, f):
    w = 2 * math.pi * f
    Z = 1 / (1j * (w*c - 1/(w * l)))
    Zo = 1 / (1/1000 + 1/Z)
    return 10 * abs(Zo / (1000 + Zo))
def calc_v_rez_B(c, l, f):
    w = 2 * math.pi * f
    P = (1j * w * l )/ (1 - (w**2 * l * c))
    H = abs(1000 / (2000 + P))
    return 10 * H


data = []


for i in range(9):
    Vs[i] *= 10**3


def create_table(Arg1, Arg2, calc):
    table = doc.add_table(rows=5, cols=len(Vs)+1)
    table.cell(0, 0).text = "F, кГц"
    table.cell(1, 0).text = "U"
    write_subscript(table.cell(1, 0), "вх")
    table.cell(2, 0).text = "U"
    write_subscript(table.cell(2, 0), "вых")
    table.cell(3, 0).text = "K = U"
    write_subscript(table.cell(3, 0), "вых")
    table.cell(3, 0).paragraphs[0].add_run("/U")
    write_subscript(table.cell(3, 0), "вх")
    table.cell(4, 0).text = "Ǩ = K/K"
    write_subscript(table.cell(4, 0), "max")
    data_local = []
    for i in range(1, len(Vs)+1):
        table.cell(0, i).text = f"{Vs[i-1]//1000}"
    for i in range(1, len(Vs)+1):
        table.cell(1, i).text = "10"
    for i in range(1, len(Vs)+1):
        table.cell(2, i).text = f"{round(calc(Arg1, Arg2, Vs[i-1]), 4)}"
    Ks = []
    for i in range(1, len(Vs)+1):
        table.cell(3, i).text = f"{round(calc(Arg1, Arg2, Vs[i-1])/ 10, 4)}"
        Ks.append(round(calc(Arg1, Arg2, Vs[i-1]), 4) / 10)
    for i in range(1, len(Vs)+1):
        table.cell(4, i).text = f"{round(Ks[i-1]/max(Ks), 4)}"
        data_local.append(float(table.cell(4, i).text))
    data.append(data_local)
    doc.save("test3.docx")
create_table(C, L, calc_v_rez_B)
doc.add_paragraph("\n")
wl.cell(row=1, column=1, value="C = 10 нФ")
for i in range(1, len(Vs)+1):
    wl.cell(row=1, column=i+1, value=data[0][i-1])
wd.save("test3.xlsx")