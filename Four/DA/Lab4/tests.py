from typing import Dict, Optional
global user_storage
user_storage: Dict[int, Dict[str, Optional[str | bool]]] = {}
def add_user(password):
    new_usr_id = list(user_storage.keys())[-1]+1 if user_storage != {} else 1
    user_storage[new_usr_id] = {"password": password, "authenticated" : False}