from my_string import MyString


value = MyString("Hello")
print(value)                 # Hello
print(value.c_str())         # Hello
print(value.size())          # 5
print(value.capacity())      # 6
print(value.empty())         # False

value.append(" World")
print(value)                 # Hello World

value.insert(5, ",")
print(value)                 # Hello, World

value[0] = "h"
print(value)                 # hello, World

part = value.substr(0, 5)
print(part)                  # hello

print(value.find("World"))  # 7
print(len(value))            # 12

value += "!"
print(value)                 # hello, World!

value.erase(5, 2)
print(value)                 # helloWorld!

value.clear()
print(value.empty())         # True

