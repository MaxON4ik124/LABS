import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--create", nargs=1)
parser.add_argument("--delete", nargs=1)
parser.add_argument("--write",nargs=2)
parser.add_argument("--read",nargs=1)
parser.add_argument("--copy", nargs=2)
parser.add_argument("--rename", nargs=2)

def f_create(path):
    try:
        file = open(path, 'x')
        file.close()
        print("[+] File successfully created")
    except FileExistsError as e:
        print(f"[-] File already exists. {e}")

def f_delete(path):
    try:
        os.remove(path)
        print("[+] File successfully deleted")
    except FileNotFoundError as e:
        print(f"[-] File {path} doesn't exist {e}")
    except IsADirectoryError as e:
        print(f"[-] Path {path} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {path}. {e}")

def f_write(path, content):
    try:
        content = content.replace("\\r\\n", "\n")
        content = content.replace("\\n", "\n")
        file = open(path, 'a')
        file.write(rf"{content}")
        file.close()
        print("[+] Content successfully written into file")
    except FileNotFoundError as e:
        print(f"[-] File {path} doesn't exist {e}")
    except IsADirectoryError as e:
        print(f"[-] Path {path} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {path}. {e}")

def f_read(path):
    try:
        file = open(path, 'r')
        content = file.read()
        print(f"[+] Content from {path} successfully extracted:")
        print(content)
        return content
    except FileNotFoundError as e:
        print(f"[-] File {path} doesn't exist {e}")
    except IsADirectoryError as e:
        print(f"[-] Path {path} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {path}. {e}")

def f_copy(src, dest):

    try:
        filename = src.split('\\')[-1]
        source = open(src, "r")
        content = source.read()
        source.close()
    except FileNotFoundError as e:
        print(f"[-] File {src} doesn't exist {e}")
        return
    except IsADirectoryError as e:
        print(f"[-] Path {src} is a directory. {e}")
        return
    except PermissionError as e:
        print(f"[-] Permission error for {src}. {e}")
        return
    try:
        destination = open(f"{dest}\\{filename}", "w")
        destination.write(content)
        destination.close()
        print(f"[+] File successfully copied from {src} to {dest}\\{filename}")
    except FileNotFoundError as e:
        print(f"[-] Invalid path {dest}. {e}")
    except UnboundLocalError:
        pass

def f_rename(src, dest):
    try:
        source = open(src, "r")
        content = source.read()
        source.close()
    except FileNotFoundError as e:
        print(f"[-] File {src} doesn't exist {e}")
        return
    except IsADirectoryError as e:
        print(f"[-] Path {src} is a directory. {e}")
        return
    except PermissionError as e:
        print(f"[-] Permission error for {src}. {e}")
        return
    try:
        destination = open(dest, "w")
        destination.write(content)
        destination.close()
        os.remove(src)
        print(f"[+] File successfully renamed from {src} to {dest}")
    except FileNotFoundError as e:
        print(f"[-] File {dest} doesn't exist {e}")
    except IsADirectoryError as e:
        print(f"[-] File {dest} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {dest}. {e}")

if __name__ == "__main__":
    args = parser.parse_args()
    match args:
        case argparse.Namespace(create=[path]):
            f_create(args.create[0])
        case argparse.Namespace(delete=[path]):
            f_delete(args.delete[0])
        case argparse.Namespace(write=[path, content]):
            f_write(args.write[0], args.write[1])
        case argparse.Namespace(read=[path]):
            f_read(args.read[0])
        case argparse.Namespace(copy=[src, dest]):
            f_copy(args.copy[0], args.copy[1])
        case argparse.Namespace(rename=[src, dest]):
            f_rename(args.rename[0], args.rename[1])