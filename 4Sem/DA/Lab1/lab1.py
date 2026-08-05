import argparse
import os
import shutil


class SafeArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        print(f"[-] {message}")
        raise ValueError(f"[-] {message}")


def ok(message):
    print(f"[+] {message}")


def print_and_raise(error_type, message):
    print(message)
    raise error_type(message)


def normalize_text(text):
    return text.replace("\\r\\n", "\n").replace("\\n", "\n").replace("\\r", "\n")


def expand_path(path):
    return os.path.expanduser(path)


def ensure_parent_dir(path):
    parent = os.path.dirname(path)
    if parent not in ("", "."):
        os.makedirs(parent, exist_ok=True)


def safe_resolve(path):
    try:
        return os.path.abspath(expand_path(path))
    except Exception:
        return expand_path(path)


def same_path(left, right):
    try:
        return os.path.normcase(safe_resolve(left)) == os.path.normcase(safe_resolve(right))
    except Exception:
        return expand_path(left) == expand_path(right)


def validate_path_arg(value, name):
    if isinstance(value, (int, float)):
        print_and_raise(ValueError, f"[-] {name} cannot be a number")

    if value is None:
        print_and_raise(ValueError, f"[-] {name} cannot be None")

    try:
        text = str(value)
    except Exception as e:
        print_and_raise(ValueError, f"[-] Cannot convert {name} to string: {e}")

    if text == "":
        print_and_raise(ValueError, f"[-] {name} cannot be empty")

    if text.strip() == "":
        print_and_raise(ValueError, f"[-] {name} cannot contain only spaces")
    if "\x00" in text:
        print(f"[-] {name} cannot contain NUL byte")

    return expand_path(text)


def validate_content_arg(value):
    if isinstance(value, (int, float)):
        print_and_raise(ValueError, "[-] Content cannot be a number")

    if value is None:
        print_and_raise(ValueError, "[-] Content cannot be None")

    try:
        text = str(value)
    except Exception as e:
        print_and_raise(ValueError, f"[-] Cannot convert Content to string: {e}")

    if "\x00" in text:
        print_and_raise(ValueError, "[-] Content cannot contain NUL byte")

    return normalize_text(text)


def require_exact_args(values, command_name, expected_count):
    actual_count = len(values)

    if actual_count < expected_count:
        print_and_raise(
            ValueError,
            f"[-] Command --{command_name} expects {expected_count} argument(s), got {actual_count}"
        )

    if actual_count > expected_count:
        print_and_raise(
            ValueError,
            f"[-] Command --{command_name} expects {expected_count} argument(s), got {actual_count}. "
            f"Possible reason: one of the arguments contains spaces or was passed incorrectly"
        )

    return values


def f_create(path_value) -> None:
    try:
        path = validate_path_arg(path_value, "Path")

        if os.path.isdir(path):
            print_and_raise(IsADirectoryError, f"[-] Path '{path}' is a directory")

        ensure_parent_dir(path)

        with open(path, "w", encoding="utf-8"):
            pass

        ok(f"File '{path}' successfully created")

    except ValueError:
        raise
    except UnicodeEncodeError:
        print(f"[-] Unicode Encode error occured.")
    except PermissionError as e:
        print_and_raise(PermissionError, f"[-] Permission denied for '{path_value}': {e}")
    except FileExistsError as e:
        print_and_raise(OSError, f"[-] File '{path_value}' already exists: {e}")
    except IsADirectoryError:
        raise
    except FileNotFoundError as e:
        print_and_raise(FileNotFoundError, f"[-] File path '{path_value}' not found: {e}")
    except OSError as e:
        print_and_raise(OSError, f"[-] OS error while creating '{path_value}': {e}")
    except Exception as e:
        print_and_raise(Exception, f"[-] Unexpected error while creating '{path_value}': {e}")


def f_delete(path_value) -> None:
    try:
        path = validate_path_arg(path_value, "Path")

        if not os.path.exists(path):
            print_and_raise(FileNotFoundError, f"[-] File '{path}' doesn't exist")

        if os.path.isdir(path):
            print_and_raise(IsADirectoryError, f"[-] Path '{path}' is a directory")

        os.remove(path)

        ok(f"File '{path}' successfully deleted")

    except ValueError:
        raise
    except FileNotFoundError:
        raise
    except IsADirectoryError:
        raise
    except UnicodeEncodeError:
        print(f"[-] Unicode Encode error occured.")
    except PermissionError as e:
        print_and_raise(PermissionError, f"[-] Permission denied for '{path_value}': {e}")
    except OSError as e:
        print_and_raise(OSError, f"[-] OS error while deleting '{path_value}': {e}")
    except Exception as e:
        print_and_raise(Exception, f"[-] Unexpected error while deleting '{path_value}': {e}")


def f_write(path_value, content_value) -> None:
    try:
        path = validate_path_arg(path_value, "Path")
        content = validate_content_arg(content_value)

        if os.path.exists(path) and os.path.isdir(path):
            print_and_raise(IsADirectoryError, f"[-] Path '{path}' is a directory")

        ensure_parent_dir(path)

        with open(path, "w", encoding="utf-8") as file:
            file.write(content)

        ok(f"Content successfully written to '{path}'")

    except UnicodeEncodeError as e:
        print(f"[-] Unicode encode error occured.")
        raise
    except UnicodeDecodeError as e:
        print(f"[-] Unicode decode error occured.")
        raise 
    except ValueError:
        raise
    except IsADirectoryError:
        raise
    except PermissionError as e:
        print_and_raise(PermissionError, f"[-] Permission denied for '{path_value}': {e}")
    except FileNotFoundError as e:
        print_and_raise(FileNotFoundError, f"[-] File '{path_value}' doesn't exist: {e}")
    except UnicodeEncodeError as e:
        print_and_raise(OSError, f"[-] Encoding error while writing to '{path_value}': {e}")
    except OSError as e:
        print_and_raise(OSError, f"[-] OS error while writing to '{path_value}': {e}")
    except Exception as e:
        print_and_raise(Exception, f"[-] Unexpected error while writing to '{path_value}': {e}")


def f_read(path_value):
    try:
        path = validate_path_arg(path_value, "Path")

        if not os.path.exists(path):
            print_and_raise(FileNotFoundError, f"[-] File '{path}' doesn't exist")

        if os.path.isdir(path):
            print_and_raise(IsADirectoryError, f"[-] Path '{path}' is a directory")

        with open(path, "r", encoding="utf-8") as file:
            content = file.read()

        ok(f"Content from '{path}' successfully extracted")
        return content
    except UnicodeEncodeError as e:
        print(f"[-] Unicode encode error occured.")
        raise
    except UnicodeDecodeError as e:
        print(f"[-] Unicode decode error occured.")
        raise 
    except ValueError:
        raise
    except FileNotFoundError:
        raise
    except IsADirectoryError:
        raise
    except UnicodeEncodeError:
        print(f"[-] Unicode Encode error occured.")
    except PermissionError as e:
        print_and_raise(PermissionError, f"[-] Permission denied for '{path_value}': {e}")
    except UnicodeDecodeError as e:
        print_and_raise(OSError, f"[-] Decoding error while reading '{path_value}': {e}")
    except OSError as e:
        print_and_raise(OSError, f"[-] OS error while reading '{path_value}': {e}")
    except Exception as e:
        print_and_raise(Exception, f"[-] Unexpected error while reading '{path_value}': {e}")


def f_copy(src_value, dest_value):
    try:
        src = validate_path_arg(src_value, "Src")
        dest = validate_path_arg(dest_value, "Dest")

        if not os.path.exists(src):
            print_and_raise(FileNotFoundError, f"[-] Src '{src}' doesn't exist")

        if os.path.isdir(src):
            print_and_raise(IsADirectoryError, f"[-] Src '{src}' is a directory")

        if os.path.exists(dest) and os.path.isdir(dest):
            print_and_raise(IsADirectoryError, f"[-] Dest '{dest}' is a directory")

        if same_path(src, dest):
            print_and_raise(OSError, "[-] Src and dest refer to the same path")

        ensure_parent_dir(dest)

        shutil.copy(src, dest)

        ok(f"File successfully copied from '{src}' to '{dest}'")

    except UnicodeEncodeError as e:
        print(f"[-] Unicode encode error occured.")
        raise
    except UnicodeDecodeError as e:
        print(f"[-] Unicode decode error occured.")
        raise    
    except ValueError:
        raise
    except FileNotFoundError:
        raise
    except IsADirectoryError:
        raise
    except PermissionError as e:
        print_and_raise(PermissionError, f"[-] Permission denied while copying '{src_value}' -> '{dest_value}': {e}")
        raise
    except UnicodeEncodeError:
        print(f"[-] Unicode Encode error occured.")
    except UnicodeDecodeError as e:
        print_and_raise(UnicodeDecodeError, f"[-] Encoding or decoding error: {e}")
    except shutil.SameFileError as e:
        print_and_raise(OSError, f"[-] Source and destination are the same file: {e}")
    except OSError as e:
        print_and_raise(OSError, f"[-] OS error while copying '{src_value}' -> '{dest_value}': {e}")
        raise
    except Exception as e:
        print_and_raise(Exception, f"[-] Unexpected error while copying '{src_value}' -> '{dest_value}': {e}")


def f_rename(src_value, dest_value):
    try:
        src = validate_path_arg(src_value, "Src")
        dest = validate_path_arg(dest_value, "Dest")

        if not os.path.exists(src):
            print_and_raise(FileNotFoundError, f"[-] Src '{src}' doesn't exist")

        if os.path.isdir(src):
            print_and_raise(IsADirectoryError, f"[-] Src '{src}' is a directory")

        if os.path.exists(dest) and os.path.isdir(dest):
            print_and_raise(IsADirectoryError, f"[-] Dest '{dest}' is a directory")

        if same_path(src, dest):
            print_and_raise(OSError, "[-] Src and dest refer to the same path")

        ensure_parent_dir(dest)

        os.rename(src, dest)

        ok(f"File successfully renamed from '{src}' to '{dest}'")

    except UnicodeEncodeError as e:
        print(f"[-] Unicode encode error occured.")
        raise
    except UnicodeDecodeError as e:
        print(f"[-] Unicode decode error occured.")
        raise 
    except ValueError:
        raise
    except FileNotFoundError:
        raise
    except IsADirectoryError:
        raise
    except PermissionError as e:
        print_and_raise(PermissionError, f"[-] Permission denied while renaming '{src_value}' -> '{dest_value}': {e}")
        raise
    except UnicodeEncodeError:
        print(f"[-] Unicode Encode error occured.")
    except UnicodeDecodeError as e:
        print_and_raise(UnicodeDecodeError, f"[-] Encoding or decoding error: {e}")
    except OSError as e:
        print_and_raise(OSError, f"[-] OS error while renaming '{src_value}' -> '{dest_value}': {e}")
        raise
    except Exception as e:
        print_and_raise(Exception, f"[-] Unexpected error while renaming '{src_value}' -> '{dest_value}': {e}")


def build_parser():
    parser = SafeArgumentParser()

    group = parser.add_mutually_exclusive_group(required=True)

    group.add_argument("--create", nargs="*")
    group.add_argument("--delete", nargs="*")
    group.add_argument("--read", nargs="*")
    group.add_argument("--write", nargs="*")
    group.add_argument("--copy", nargs="*")
    group.add_argument("--rename", nargs="*")

    return parser


def main():
    parser = build_parser()
    args = parser.parse_args()

    if args.create is not None:
        values = require_exact_args(args.create, "create", 1)
        f_create(values[0])
        return

    if args.delete is not None:
        values = require_exact_args(args.delete, "delete", 1)
        f_delete(values[0])
        return

    if args.read is not None:
        values = require_exact_args(args.read, "read", 1)
        content = f_read(values[0])
        print(content)
        return

    if args.write is not None:
        values = require_exact_args(args.write, "write", 2)
        f_write(values[0], values[1])
        return

    if args.copy is not None:
        values = require_exact_args(args.copy, "copy", 2)
        f_copy(values[0], values[1])
        return

    if args.rename is not None:
        values = require_exact_args(args.rename, "rename", 2)
        f_rename(values[0], values[1])
        return

    print_and_raise(ValueError, "[-] No operation selected")

if __name__ == "__main__":
    main()