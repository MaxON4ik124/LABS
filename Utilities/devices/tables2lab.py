from docx import Document
import math

doc = Document()
Rs = [1200, 1400, 1600]
Cs = [10, 20, 40]
for i in range(3):
    Cs[i] *= 10**-9
Freq = [100, 300, 500, 1000, 1500, 2000, 5000, 15000]
def calc_V(r, c, v):
    print(c)
    Rc = 1/(2*math.pi*v*c)
    Ro = r*2 + Rc
    I = 2/Ro
    print(Rc, Ro, I, I*Rc, 2*I*r)
    return I*Rc 
print(calc_V(1800, 100*10**-9, 100))