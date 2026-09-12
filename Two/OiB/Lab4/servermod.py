import socket
import sys
import os
import time

def main():
    SERVER_HOST = '0.0.0.0'  
    SERVER_PORT = 9999      
    
    print(f"[*] Starting server on {SERVER_HOST}:{SERVER_PORT}")
    
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server_socket.bind((SERVER_HOST, SERVER_PORT))
        
        server_socket.listen(5)
        print("[*] Server is running and waiting for connections...")
        
        while True:
            client_socket, client_address = server_socket.accept()
            print(f"[+] Connection established with {client_address[0]}:{client_address[1]}")
            
            handle_client(client_socket)
            
    except KeyboardInterrupt:
        print("\n[*] Shutting down server...")
    except Exception as e:
        print(f"[!] Error: {e}")
    finally:
        server_socket.close()
        print("[*] Server stopped")

def handle_client(client_socket):
    try:
        client_info = client_socket.recv(1024).decode('utf-8')
        print(f"[*] Information from client: {client_info}")
        
        file_to_delete = input("[>] Enter filename to delete on client: ")
        
        print(f"[*] Sending delete file command: {file_to_delete}")
        client_socket.send(f"DELETE:{file_to_delete}".encode('utf-8'))
        
        response = client_socket.recv(1024).decode('utf-8')
        print(f"[*] Response from client: {response}")
        
    except Exception as e:
        print(f"[!] Error while handling client: {e}")
    finally:
        client_socket.close()

if __name__ == "__main__":
    main()