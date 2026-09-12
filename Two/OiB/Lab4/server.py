import socket
import sys

def start_server():
    host = input("Enter the client's IP address: ")
    port = 9999
    
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((host, port))
        
        filename = input("Enter the path to the file to delete: ")
        client_socket.send(filename.encode('utf-8'))
        
        response = client_socket.recv(1024).decode('utf-8')
        print(f"[*] Response from client: {response}")
        
        client_socket.close()
        
    except Exception as e:
        print(f"[!] Error: {str(e)}")

if __name__ == "__main__":
    start_server()