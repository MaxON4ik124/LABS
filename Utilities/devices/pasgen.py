import random
table = '1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM!@#$%^&*'
pas = ''
for i in range(30):
    pas = pas + random.choice(table)
print(pas)