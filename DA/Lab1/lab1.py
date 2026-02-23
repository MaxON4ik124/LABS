import os
import argparse
import shutil

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
        print(f"[-] File {path} already exists. {e}")

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
        file = open(path, 'xa')
        file.write(f"{content}")
        file.close()
        print(f"[+] Content successfully written into {path}")
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
        shutil.copyfile(src, dest)
        print(f"[+] File successfully copied from {src} to {dest}")
    except FileNotFoundError as e:
        print(f"[-] File {src} doesn't exist {e}")
    except IsADirectoryError as e:
        print(f"[-] Path {src} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {src}. {e}")

def f_rename(src, dest):
    try:
        os.rename(src, dest)
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