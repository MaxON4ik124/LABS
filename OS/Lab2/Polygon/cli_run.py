import cligen
import os
def main():
    clients, msgs = map(int, input("Enter count of clients and count of messages:").split())
    for i in range(1, clients+1):
        for g in range(1, msgs+1):
            cligen.write_to_file(f"file_{i}.txt", f"{i}___msg{g}")
        if i == clients:
            cligen.write_to_file(f"file_{clients+1}.txt", "stop")
        os.system(f".\\tcpclient 192.168.67.130:9005 file_{i}.txt")
    os.system(f".\\tcpclient 192.168.67.130:9005 file_{clients+1}.txt")

main()