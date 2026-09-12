import socket
import os
import sys

def delete_file(filename):
    try:
        if os.path.exists(filename):
            os.remove(filename)
            return f"File {filename} successfully deleted."
        else:
            return f"File {filename} not found."
    except Exception as e:
        return f"Error when deleting file: {str(e)}"

def start_client():
    host = '169.254.174.129'
    port = 9999
    
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server_socket.bind((host, port))
        server_socket.listen(1)
        print(f"[*] Client started and waiting for connections on port {port}")
        
        while True:
            client_socket, address = server_socket.accept()
            print(f"[*] Connection received from {address[0]}:{address[1]}")
            
            filename = client_socket.recv(1024).decode('utf-8')
            print(f"[*] Received command to delete file: {filename}")
            
            result = delete_file(filename)
            client_socket.send(result.encode('utf-8'))
            
            client_socket.close()
            
    except Exception as e:
        print(f"[!] Error: {str(e)}")
    finally:
        server_socket.close()

if __name__ == "__main__":
    start_client()