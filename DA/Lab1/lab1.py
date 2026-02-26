import os
import argparse
import re

parser = argparse.ArgumentParser()
parser.add_argument("--create", nargs='*', default=False)
parser.add_argument("--delete", nargs='*', default=False)
parser.add_argument("--write",nargs='*', default=False)
parser.add_argument("--read",nargs='*', default=False)
parser.add_argument("--copy", nargs='*', default=False)
parser.add_argument("--rename", nargs='*', default=False)

def f_create(path):
    try:

        dirs = re.split(r'/|\\', path)
        for i in range(len(dirs) - 1):
            if not os.path.exists(dirs[i]):
                os.mkdir(dirs[i])
            os.chdir(dirs[i])
        file = open(dirs[-1], 'x')
        file.close()
        print(f"[+] File {path} successfully created")
        print(f"[+] {path} successfully created")
    except FileExistsError as e:
        print(f"[-] {path} already exists. {e}")
    except UnboundLocalError as e:
        print(f"Invalid path: {path}. {e}")
    except IsADirectoryError as e:
        print(f"[-] Path {path} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {path}. {e}")
    except OSError as e:
        print(f"[-] OS error for {path}. {e}")
    except ValueError as e:
        print(f"[-] Value error for {path}")

def f_delete(path):
    try:
        os.remove(path)
        print(f"[+] {path} successfully deleted")
    except FileNotFoundError as e:
        print(f"[-] File {path} doesn't exist {e}")
    except IsADirectoryError as e:
        print(f"[-] Path {path} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {path}. {e}")
    except OSError as e:
        print(f"[-] OS Error for {path}. {e}")
    except ValueError as e:
        print(f"[-] Value error for {path}")

def f_write(path, content):
    try:
        if not os.path.exists(path):
            raise FileNotFoundError
        content = content.replace("\\r\\n", "\n")
        content = content.replace("\\n", "\n")
<<<<<<< HEAD
        if content is None:
            raise ValueError("[-] Content cannot be None")
        if not os.path.exists(path):
            raise FileNotFoundError
        file = open(path, 'a')
        file.write(f"{content}")
=======
        file = open(path, 'w')
        file.write(rf"{content}")
>>>>>>> c7b168f4bea8dc219176bfcb9cbdfd338f8319b7
        file.close()
        print(f"[+] Content successfully written to {path}")
    except FileNotFoundError as e:
        print(f"[-] File {path} doesn't exist {e}")
    except IsADirectoryError as e:
        print(f"[-] Path {path} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {path}. {e}")
    except OSError as e:
        print(f"[-] OS Error for {path}. {e}")
    except ValueError as e:
        print(f"[-] Value error for {path}")
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
    except OSError as e:
        print(f"[-] OS Error for {path}. {e}")
    except ValueError as e:
        print(f"[-] Value error for {path}")


def f_copy(src, dest):
    try:
        assert(src != dest)
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
    except OSError as e:
        print(f"[-] OS Error for {src}. {e}")
        return
    except ValueError as e:
        print(f"[-] Value error for {src}. {e}")
        return
    try:
        destination = open(f"{dest}", "w")
        destination.write(content)
        destination.close()
        print(f"[+] File successfully copied from {src} to {dest}")
    except FileNotFoundError as e:
        print(f"[-] Invalid path {dest}. {e}")
    except UnboundLocalError:
        print(f"Unbound Local Error for {dest}. {e}")
    except OSError as e:
        print(f"[-] OS Error for {dest}. {e}")
    except ValueError as e:
        print(f"[-] Value error for {dest}. {e}")
    except IsADirectoryError as e:
        print(f"[-] Path {src} is a directory. {e}")
    except PermissionError as e:
        print(f"[-] Permission error for {src}. {e}")


def f_rename(src, dest):
    try:
        assert(src != dest)
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
    except OSError as e:
        print(f"[-] OS Error for {src}. {e}")
        return
    except ValueError as e:
        print(f"[-] Value error for {src}. {e}")
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
    except OSError as e:
        print(f"[-] OS Error for {dest}. {e}")
    except ValueError as e:
        print(f"[-] Value error for {dest}. {e}")
if __name__ == "__main__":
    args = parser.parse_args()
    if args.create != False:
        if len(args.create) < 1:
            raise ValueError("[-] Path cannot be None")
        else:
            f_create(args.create[0])
    elif args.delete != False:
        if len(args.delete) < 1:
            raise ValueError("[-] Path cannot be None")
        else:
            f_delete(args.delete[0])
    elif args.write != False:
        if len(args.write) == 1:
            raise ValueError("[-] Content cannot be None")
        elif len(args.write) < 1:
            raise ValueError("[-] Path cannot be None")
        else:
            f_write(args.write[0], args.write[1])
    elif args.read != False:
        if len(args.read) < 1:
            raise ValueError("[-] Path cannot be None")
        else:
            f_read(args.read[0])
    elif args.copy != False:
        if len(args.copy) == 1:
            raise ValueError("[-] Dest cannot be None")
        elif len(args.copy) < 1:
            raise ValueError("[-] Src cannot be None")
        else:
            f_copy(args.copy[0], args.copy[1])
    elif args.rename != False:
        if len(args.rename) == 1:
            raise ValueError("[-] Dest cannot be None")
        elif len(args.rename) < 1:
            raise ValueError("[-] Src cannot be None")
        else:
            f_rename(args.rename[0], args.rename[1])