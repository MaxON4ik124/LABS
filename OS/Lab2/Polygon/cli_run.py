import cilfilegen
import os
def main():
    clients, msgs = map(int, input("Enter count of clients and count of messages:").split())
    for i in range(1, clients+1):
        for g in range(1, msgs+1):
            cilfilegen.write_to_file(f"file_{i}.txt", f"{i}___msg{g}")
        if i == msgs:
            cilfilegen.write_to_file(f"file{clients+2}", "stop")
        os.system(f"./udpclient 192.168.56.1:9005 file{i}.txt")
    os.system(f"./udpclient 192.168.56.1:9005 file{clients+2}.txt")

if __name__ == "__main__":
    main()