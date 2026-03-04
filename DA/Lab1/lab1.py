import argparse
import os
import shutil
import sys
from pathlib import Path


def ok(msg: str) -> None:
    print(f"[+] {msg}")

def err(msg: str) -> None:
    print(f"[-] {msg}")

def normalize_text(s: str) -> str:
    return s.replace("\\r\\n", "\n").replace("\\n", "\n").replace("\\r", "\n")

def ensure_parent_dir(path: Path) -> None:
    parent = path.parent
    if parent and str(parent) not in ("", "."):
        parent.mkdir(parents=True, exist_ok=True)

def safe_resolve(p: Path) -> Path:
    try:
        return p.expanduser().resolve(strict=False)
    except Exception:
        return p.expanduser()

def same_path(a: Path, b: Path) -> bool:
    ra = safe_resolve(a)
    rb = safe_resolve(b)
    try:
        return os.path.normcase(str(ra)) == os.path.normcase(str(rb))
    except Exception:
        return str(ra) == str(rb)

def fail_and_exit(message: str, code: int = 2) -> None:
    err(message)
    sys.exit(code)


def f_create(path_str: str) -> int:
    try:
        if not path_str or not path_str.strip():
            err("Path cannot be empty")
            return 2

        path = Path(path_str)
        path = path.expanduser()

        if path.exists() and path.is_dir():
            err(f"Path '{path}' is a directory")
            return 2

        ensure_parent_dir(path)

        with open(path, "x", encoding="utf-8"):
            pass

        ok(f"File '{path}' successfully created")
        return 0

    except FileExistsError:
        err(f"File '{path_str}' already exists")
        return 2
    except PermissionError as e:
        err(f"Permission error for '{path_str}': {e}")
        return 2
    except IsADirectoryError as e:
        err(f"Path '{path_str}' is a directory: {e}")
        return 2
    except OSError as e:
        err(f"OS error while creating '{path_str}': {e}")
        return 2
    except Exception as e:
        err(f"Unexpected error while creating '{path_str}': {e}")
        return 2


def f_delete(path_str: str) -> int:
    try:
        if not path_str or not path_str.strip():
            err("Path cannot be empty")
            return 2

        path = Path(path_str).expanduser()

        if not path.exists():
            err(f"File '{path}' doesn't exist")
            return 2
        if path.is_dir():
            err(f"Path '{path}' is a directory (delete supports files only)")
            return 2

        path.unlink()
        ok(f"File '{path}' successfully deleted")
        return 0

    except PermissionError as e:
        err(f"Permission error for '{path_str}': {e}")
        return 2
    except OSError as e:
        err(f"OS error while deleting '{path_str}': {e}")
        return 2
    except Exception as e:
        err(f"Unexpected error while deleting '{path_str}': {e}")
        return 2


def f_write(path_str: str, content: str) -> int:
    try:
        if not path_str or not path_str.strip():
            err("Path cannot be empty")
            return 2
        if content is None:
            err("Content cannot be None")
            return 2

        path = Path(path_str).expanduser()

        if not path.exists():
            err(f"File '{path}' doesn't exist")
            return 2
        if path.is_dir():
            err(f"Path '{path}' is a directory")
            return 2

        text = normalize_text(content)

        with open(path, "a", encoding="utf-8", errors="strict") as f:
            f.write(text)

        ok(f"Content successfully written to '{path}'")
        return 0

    except UnicodeEncodeError as e:
        err(f"Encoding error while writing '{path_str}': {e}")
        return 2
    except PermissionError as e:
        err(f"Permission error for '{path_str}': {e}")
        return 2
    except OSError as e:
        err(f"OS error while writing '{path_str}': {e}")
        return 2
    except Exception as e:
        err(f"Unexpected error while writing '{path_str}': {e}")
        return 2


def f_read(path_str: str) -> int:
    try:
        if not path_str or not path_str.strip():
            err("Path cannot be empty")
            return 2

        path = Path(path_str).expanduser()

        if not path.exists():
            err(f"File '{path}' doesn't exist")
            return 2
        if path.is_dir():
            err(f"Path '{path}' is a directory")
            return 2

        with open(path, "r", encoding="utf-8", errors="strict") as f:
            content = f.read()

        ok(f"Content from '{path}' successfully extracted:")
        print(content)
        return 0

    except UnicodeDecodeError as e:
        err(f"Decoding error while reading '{path_str}': {e}")
        return 2
    except PermissionError as e:
        err(f"Permission error for '{path_str}': {e}")
        return 2
    except OSError as e:
        err(f"OS error while reading '{path_str}': {e}")
        return 2
    except Exception as e:
        err(f"Unexpected error while reading '{path_str}': {e}")
        return 2


def f_copy(src_str: str, dest_str: str) -> int:
    try:
        if not src_str or not src_str.strip():
            err("Src cannot be empty")
            return 2
        if not dest_str or not dest_str.strip():
            err("Dest cannot be empty")
            return 2

        src = Path(src_str).expanduser()
        dest = Path(dest_str).expanduser()

        if not src.exists():
            err(f"File '{src}' doesn't exist")
            return 2
        if src.is_dir():
            err(f"Src '{src}' is a directory (copy supports files only)")
            return 2
        if dest.exists() and dest.is_dir():
            err(f"Dest '{dest}' is a directory")
            return 2

        if same_path(src, dest):
            err("Src and dest refer to the same path")
            return 2

        ensure_parent_dir(dest)

        shutil.copy2(src, dest)
        ok(f"File successfully copied from '{src}' to '{dest}'")
        return 0

    except PermissionError as e:
        err(f"Permission error while copying '{src_str}' -> '{dest_str}': {e}")
        return 2
    except OSError as e:
        err(f"OS error while copying '{src_str}' -> '{dest_str}': {e}")
        return 2
    except Exception as e:
        err(f"Unexpected error while copying '{src_str}' -> '{dest_str}': {e}")
        return 2


def f_rename(src_str: str, dest_str: str) -> int:
    try:
        if not src_str or not src_str.strip():
            err("Src cannot be empty")
            return 2
        if not dest_str or not dest_str.strip():
            err("Dest cannot be empty")
            return 2

        src = Path(src_str).expanduser()
        dest = Path(dest_str).expanduser()

        if not src.exists():
            err(f"File '{src}' doesn't exist")
            return 2
        if src.is_dir():
            err(f"Src '{src}' is a directory (rename supports files only)")
            return 2
        if dest.exists() and dest.is_dir():
            err(f"Dest '{dest}' is a directory")
            return 2

        if same_path(src, dest):
            err("Src and dest refer to the same path")
            return 2

        ensure_parent_dir(dest)

        os.replace(src, dest)
        ok(f"File successfully renamed from '{src}' to '{dest}'")
        return 0

    except PermissionError as e:
        err(f"Permission error while renaming '{src_str}' -> '{dest_str}': {e}")
        return 2
    except OSError as e:
        err(f"OS error while renaming '{src_str}' -> '{dest_str}': {e}")
        return 2
    except Exception as e:
        err(f"Unexpected error while renaming '{src_str}' -> '{dest_str}': {e}")
        return 2


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()

    group = parser.add_mutually_exclusive_group(required=True)

    group.add_argument("--create", metavar="PATH")
    group.add_argument("--delete", metavar="PATH")
    group.add_argument("--read", metavar="PATH")
    group.add_argument("--write", nargs=2, metavar=("PATH", "CONTENT"))
    group.add_argument("--copy", nargs=2, metavar=("SRC", "DEST"))
    group.add_argument("--rename", nargs=2, metavar=("SRC", "DEST"))

    return parser


def main() -> int:
    parser = build_parser()

    try:
        args = parser.parse_args()

        if args.create is not None:
            return f_create(args.create)

        if args.delete is not None:
            return f_delete(args.delete)

        if args.read is not None:
            return f_read(args.read)

        if args.write is not None:
            path, content = args.write
            return f_write(path, content)

        if args.copy is not None:
            src, dest = args.copy
            return f_copy(src, dest)

        if args.rename is not None:
            src, dest = args.rename
            return f_rename(src, dest)

        err("No operation selected")
        return 2

    except SystemExit:
        raise
    except KeyboardInterrupt:
        err("Interrupted by user")
        return 130
    except Exception as e:
        err(f"Fatal unexpected error: {e}")
        return 2


if __name__ == "__main__":
    sys.exit(main())