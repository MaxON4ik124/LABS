import cligen
import os
import sys
def main():
    clients, msgs = map(int, input("Enter count of clients and count of messages:").split())
    for i in range(1, clients+1):
        # for g in range(1, msgs+1):
        #     cligen.write_to_file(f"file_{i}.txt", f"{i}___msg{g}")
        # if i == clients:
        #     cligen.write_to_file(f"file_{clients+1}.txt", "stop")
        if sys.platform == "win32":
            os.system(f"ruby tcpclientemul.rb 192.168.67.130:9000 file_{i}.txt")
        elif sys.platform == "linux":
            os.system(f"./udpclient 192.168.1.82:9005 file_{i}.txt")
    if sys.platform == "win32":
        os.system(f"ruby tcpclientemul.rb 192.168.67.130:9000 file_{clients+1}.txt")
    elif sys.platform == "linux":
        os.system(f"./udpclient 192.168.1.82:9005 file_{clients+1}.txt")


main()