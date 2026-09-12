from tkinter import *
from tkinter import font
from datetime import datetime


root = Tk()
timee = 0
f = -1
after_id = ''

en = -10

def activate():

    global timee, f, after_id, en

    if timee % 20 == 0:
        f += 1
        en += 10
        TotalEnergy.config(text=f"Всего выработанно {en} кВт * ч ")
    if f % 3 == 0:
        Fact1.config(image=FactOn) 
        Fact3.config(image=FactOn) 
        Fact2.config(image=FactOff) 
        # Fact4.config(image=FactOff)
    if f % 3 == 1:
        Fact2.config(image=FactOn) 
        Fact3.config(image=FactOn) 
        Fact1.config(image=FactOff) 
        # Fact4.config(image=FactOff)
    if f % 3 == 2:
        Fact1.config(image=FactOn) 
        Fact2.config(image=FactOn) 
        Fact3.config(image=FactOff) 
        # Fact3.config(image=FactOff)
    after_id = root.after(1000, activate)
    f_temp = datetime.fromtimestamp(timee).strftime("%M:%S")
    Timer.config(text=str(f_temp))
    timee += 1

def stop():
    root.after_cancel(after_id)



root.geometry('650x650')
font1 = font.Font(family= "Arial", size=20 , weight='bold')
FactOn = PhotoImage(file="..\\Factory\\FactoryON.png")
FactOff = PhotoImage(file="..\\Factory\\FactoryOFF.png",)
Start = Button(root, width=10, height=2, fg='green', text='Начать',font=(35), command=activate)
Stop = Button(root, width=10, height=2, fg='red', text='Стоп',font=(35),command=stop)
Timer = Label(root, text="00:00", font=font1, relief=RAISED, width=10, height=2)
TotalEnergy = Label(root, width=25, height=2, text=" Всего выработанно 0 кВт * ч ", relief=RAISED, font=font1)
Fact1 = Label(image=FactOff)
Fact1.place(x=150,y=90)
Fact2 = Label(image=FactOff)
Fact2.place(x=350,y=90)
Fact3 = Label(image=FactOff)
Fact3.place(x=150,y=340)
Fact4 = Label(image=FactOff)
Fact4.place(x=350,y=340)
Start.place(x=170, y=520)
Stop.place(x=370, y=520)
Timer.place(x=20, y=20)
TotalEnergy.place(x=220, y=20)

        


root.mainloop()
