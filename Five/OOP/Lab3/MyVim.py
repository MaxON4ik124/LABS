
from pathlib import Path
import os
import shutil
import sys
import tempfile

from my_string import MyString


def validate_text(text: str) -> None:
    if any(ch not in "\n\t" and not " " <= ch <= "~" for ch in text):
        raise ValueError("Only printable ASCII, tabs and line breaks are supported")


class Document:
    def __init__(self, text: str = "") -> None:
        validate_text(text)

        self.lines = [MyString(line) for line in text.split("\n")]
        self.path: Path | None = None
        self.newline = "\n"
        self.modified = False

    @classmethod
    def open(cls, path: Path) -> "Document":
        path = path.resolve()
        if not path.exists():
            document = cls()
        else:
            raw = path.read_bytes().decode("ascii")
            newline = "\r\n" if "\r\n" in raw else "\n"
            if newline == "\r\n" and "\n" in raw.replace("\r\n", ""):
                raise ValueError("Mixed LF/CRLF line endings are not supported")
            document = cls(raw.replace("\r\n", "\n"))
            document.newline = newline
        document.path = path
        return document

    def text(self) -> str:
        return "\n".join(str(line) for line in self.lines)

    def insert(self, row: int, column: int, text: str) -> None:
        validate_text(text)
        if "\n" in text:
            raise ValueError("Use split_line for line breaks")
        if text:
            self.lines[row].insert(column, text)
            self.modified = True

    def split_line(self, row: int, column: int) -> None:
        text = str(self.lines[row])
        self.lines[row] = MyString(text[:column])
        self.lines.insert(row + 1, MyString(text[column:]))
        self.modified = True

    def delete_character(self, row: int, column: int) -> None:
        if column < len(self.lines[row]):
            self.lines[row].erase(column, 1)
            self.modified = True

    def backspace(self, row: int, column: int) -> tuple[int, int]:
        if column:
            self.delete_character(row, column - 1)
            return row, column - 1
        if row:
            previous_length = len(self.lines[row - 1])
            self.lines[row - 1].append(str(self.lines[row]))
            del self.lines[row]
            self.modified = True
            return row - 1, previous_length
        return row, column

    def delete_line(self, row: int) -> None:
        if len(self.lines) == 1:
            if self.lines[0].empty():
                return
            self.lines[0] = MyString("")
        else:
            del self.lines[row]
        self.modified = True

    def save(self, path: Path | None = None) -> None:
        target = path.resolve() if path is not None else self.path
        if target is None:
            raise ValueError("No file name; use :w path")
        data = self.text().replace("\n", self.newline).encode("ascii")

        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=target.parent, delete=False) as file:
                temporary = Path(file.name)
                file.write(data)
                file.flush()
                os.fsync(file.fileno())
            os.replace(temporary, target)
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)
        self.path = target
        self.modified = False


class Editor:


    def __init__(self, document: Document) -> None:
        self.document = document
        self.row = self.column = 0
        self.mode = "NORMAL"
        self.command = ""
        self.pending = ""
        self.message = "i: insert | :w: save | :q: quit"
        self.running = True
        self.preferred_column = 0

    def clamp(self) -> None:
        self.row = max(0, min(self.row, len(self.document.lines) - 1))
        length = len(self.document.lines[self.row])
        limit = length if self.mode == "INSERT" else max(0, length - 1)
        self.column = max(0, min(self.column, limit))

    def execute_command(self) -> None:
        parts = self.command.strip().split(maxsplit=1)
        name = parts[0] if parts else ""
        argument = parts[1] if len(parts) == 2 else None
        try:
            if name in {"w", "wq"}:
                self.document.save(Path(argument) if argument else None)
                self.message = "Saved"
                if name == "wq":
                    self.running = False
            elif name in {"q", "q!"} and argument is None:
                if self.document.modified and name != "q!":
                    self.message = "Unsaved changes; use :w or :q!"
                else:
                    self.running = False
            elif name:
                self.message = "Unknown command: " + self.command
        except (OSError, ValueError) as error:
            self.message = str(error)
        self.command = ""
        self.mode = "NORMAL"
        self.clamp()

    def handle_key(self, key: str) -> None:
        last_space = self.document.lines[self.row].rfind(" ")
        first_space = self.document.lines[self.row].find(" ")
        if key == "ESC":
            self.mode = "NORMAL"
            self.pending = self.command = ""
            self.clamp()
            self.preferred_column = self.column
            return
        if self.mode == "COMMAND":
            if key == "ENTER":
                self.execute_command()
            elif key == "BACKSPACE":
                self.command = self.command[:-1]
            elif len(key) == 1 and key.isprintable():
                self.command += key
            return

        directions = {"LEFT": (0, -1), "RIGHT": (0, 1),
                      "UP": (-1, 0), "DOWN": (1, 0)}
        if key in directions:
            self.pending = ""
            dr, dc = directions[key]
            self.row += dr
            self.column = self.preferred_column if dr else self.column + dc
            self.clamp()
            if not dr:
                self.preferred_column = self.column
            return

        if self.mode == "INSERT":
            if key == "ENTER":
                self.document.split_line(self.row, self.column)
                self.row += 1
                self.column = 0
            elif key == "BACKSPACE":
                self.row, self.column = self.document.backspace(self.row, self.column)
            elif key == "DELETE":
                self.document.delete_character(self.row, self.column)
            elif key == "TAB" or (len(key) == 1 and " " <= key <= "~"):
                text = "    " if key == "TAB" else key
                self.document.insert(self.row, self.column, text)
                self.column += len(text)
            elif len(key) == 1:
                self.message = "ASCII input only"
        else:
            pending, self.pending = self.pending, ""
            if key == "0":
                self.column = 0
            elif key == "$":
                self.column = len(self.document.lines[self.row])
            elif key == "w":
                near_space = self.document.lines[self.row].find(" ", self.column+2)
                if(self.column > last_space):
                    self.column = len(self.document.lines[self.row])
                else:
                    if(near_space == first_space):
                        self.column = len(self.document.lines[self.row])
                    else:
                        self.column = near_space - 1
            elif key == "b":
                near_space = self.document.lines[self.row].rfind(" ", 0, self.column-1)
                if(self.column < first_space):
                    self.column = 0
                else:
                    if(near_space == first_space):
                        self.column = first_space + 1
                    else:
                        self.column = near_space + 1
            elif key in {"i", "a"}:
                self.mode = "INSERT"
            elif key == "d":
                if pending == "d":
                    self.document.delete_line(self.row)
                else:
                    self.pending = "d"
            elif key == "x":
                self.document.delete_character(self.row, self.column)
                if key == "a" and len(self.document.lines[self.row]):
                    self.column += 1
            elif key == ":":
                self.mode = "COMMAND"
                self.command = ""
        self.clamp()
        self.preferred_column = self.column


def safe_screen_text(text: str) -> str:

    return "".join(ch if " " <= ch <= "~" else "?" for ch in text)


class WindowsTerminal:
    def __enter__(self) -> "WindowsTerminal":
        import ctypes
        from ctypes import wintypes

        if not sys.stdin.isatty() or not sys.stdout.isatty():
            raise OSError("Run in an interactive Windows terminal, not redirected I/O")
        self.kernel = ctypes.WinDLL("kernel32", use_last_error=True)
        self.kernel.GetStdHandle.argtypes = [wintypes.DWORD]
        self.kernel.GetStdHandle.restype = wintypes.HANDLE
        self.kernel.GetConsoleMode.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD)]
        self.kernel.SetConsoleMode.argtypes = [wintypes.HANDLE, wintypes.DWORD]
        self.handle = self.kernel.GetStdHandle(-11 & 0xFFFFFFFF)
        self.original_mode = wintypes.DWORD()
        if not self.kernel.GetConsoleMode(self.handle, ctypes.byref(self.original_mode)):
            raise ctypes.WinError(ctypes.get_last_error())
        if not self.kernel.SetConsoleMode(self.handle, self.original_mode.value | 0x0004):
            raise ctypes.WinError(ctypes.get_last_error())
        sys.stdout.write("\x1b[?1049h\x1b[?25l")
        sys.stdout.flush()
        return self

    def __exit__(self, *args: object) -> None:
        try:
            sys.stdout.write("\x1b[0m\x1b[?25h\x1b[?1049l")
            sys.stdout.flush()
        finally:
            self.kernel.SetConsoleMode(self.handle, self.original_mode.value)

    @staticmethod
    def read_key() -> str:
        import msvcrt

        key = msvcrt.getwch()
        if key in {"\x00", "\xe0"}:
            return {"K": "LEFT", "M": "RIGHT", "H": "UP", "P": "DOWN",
                    "S": "DELETE"}.get(msvcrt.getwch(), "UNKNOWN")
        return {"\x1b": "ESC", "\r": "ENTER", "\b": "BACKSPACE",
                "\t": "TAB", "\x03": "ESC"}.get(key, key)


class TerminalView:
    def __init__(self) -> None:
        self.top = self.left = 0

    def draw(self, editor: Editor) -> None:
        width, height = shutil.get_terminal_size((80, 24))
        width = max(1, width - 1)
        body_height = max(1, height - 2)
        line = str(editor.document.lines[editor.row])
        screen_column = len(line[:editor.column].expandtabs(4))

        self.top = min(self.top, editor.row)
        self.top = max(self.top, editor.row - body_height + 1)
        self.left = min(self.left, screen_column)
        self.left = max(self.left, screen_column - width + 1)

        output = ["\x1b[?25l"]

        for screen_row in range(body_height):
            row = self.top + screen_row
            text = (str(editor.document.lines[row]).expandtabs(4)
                    if row < len(editor.document.lines) else "~")
            output.append(f"\x1b[{screen_row + 1};1H\x1b[2K" + text[self.left:self.left + width])

        path = str(editor.document.path) if editor.document.path else "[New file]"

        status = (f"{editor.mode} | {path} {'[+]' if editor.document.modified else ''}"
                  f" | {editor.row + 1}:{editor.column + 1} {editor.pending}")
        
        output.append(f"\x1b[{body_height + 1};1H\x1b[2K\x1b[7m"
                      + safe_screen_text(status)[:width].ljust(width) + "\x1b[0m")
        message = ":" + editor.command if editor.mode == "COMMAND" else editor.message

        output.append(f"\x1b[{body_height + 2};1H\x1b[2K" + safe_screen_text(message)[-width:])

        if editor.mode == "COMMAND":
            cursor_row, cursor_column = body_height + 2, min(len(message) + 1, width)
        else:
            cursor_row = editor.row - self.top + 1
            cursor_column = screen_column - self.left + 1

        output.append(f"\x1b[{cursor_row};{cursor_column}H\x1b[?25h")

        sys.stdout.write("".join(output))
        sys.stdout.flush()


def main() -> int:
    if os.name != "nt":
        print("This terminal adapter requires Windows.", file=sys.stderr)
        return 1
    if len(sys.argv) > 2:
        print("Usage: python MyVim.py [file]", file=sys.stderr)
        return 1
    try:
        document = Document.open(Path(sys.argv[1])) if len(sys.argv) == 2 else Document()
        editor = Editor(document)
        view = TerminalView()
        with WindowsTerminal() as terminal:
            while editor.running:
                view.draw(editor)
                editor.handle_key(terminal.read_key())
    except (OSError, ValueError) as error:
        print(f"MyVim: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())