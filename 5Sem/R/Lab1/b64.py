import base64
with open("tigran.png", "rb") as img:
    b64 = base64.b64encode(img.read()).decode("utf-8")

with open("tigran.txt", "w") as file:
    file.write(b64)


with open("tigran.txt", "r") as file:
    imgb64 = file.read()

image_data = base64.b64decode(imgb64)
with open("tigran_decoded.png", "wb") as img:
    img.write(image_data)