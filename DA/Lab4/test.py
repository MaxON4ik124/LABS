import tests
# from typing import Dict, Optional

# Users = tests.user_storage
# for i in range(5):
#     pwd = input()
#     tests.add_user(pwd)
#     print(Users)

# print("Login")
# a = input()

x = {1: "a", 2: [1, 3, 2]}
print(list(x.keys()))
a = x[2].pop()
print(a)