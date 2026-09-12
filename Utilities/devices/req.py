import requests


test = requests.get("https://38.244.216.208:8419/r2aUKmFvnKNGjdNV6A/", timeout=10)
print(test.status_code)