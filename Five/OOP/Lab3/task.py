# ------------------------------------------------------w----------------------------------------------------
Task1 = "Word Word Word Word Word Word CurWord NextWord Word Word Word" # Сейчас курсор на C. Надо чтобы на d
row, col = 0, 30
space = Task1.find(" ", col+1)
col = space - 1
print("Task1:", Task1[col])   
# ------------------------------------------------------b-----------------------------------------------------
Task2 = "Word Word Word Word Word Word CurWord NextWord Word Word Word" # Сейчас курсор на r. Надо чтобы на C
row, col = 0, 32
col = Task2.rfind(" ", 0, col-1) + 1
print("Task2:", Task2[col])