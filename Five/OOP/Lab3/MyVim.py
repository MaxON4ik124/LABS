
from pathlib import Path
import os
import shutil
import sys
import tempfile

from my_string import MyString


HELP_PATH = Path(__file__).with_name("help.txt")


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
        self.buffer = MyString("")

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

    def delete_word(self, row: int, column: int) -> None:
        offs = 1
        if column < len(self.lines[row]):
            spaces = [self.lines[row].find(" ", column), self.lines[row].rfind(" ", 0, column)]
            if(spaces[1] == 0): spaces[1] -= 1
            elif(spaces[0] == 0): spaces[0] = len(self.lines[row])-1
            else: offs = 0
            word = spaces[0] - spaces[1]
            self.lines[row].erase(spaces[1]+offs, word)
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

    def copy_line(self, row: int) -> None:
        self.buffer = MyString(str(self.lines[row]))
        self.buffer.append('\n')
        self.modified = True

    def copy_word(self, row: int, column: int) -> None:
        if column < len(self.lines[row]):
            text = str(self.lines[row])
            start = text.rfind(" ", 0, column) + 1
            end = text.find(" ", column)
            if end == -1:
                end = len(text)
            self.buffer = MyString(text[start:end])
            self.modified = True

    def paste(self, row: int, column: int) -> tuple[int, int]:
        text = str(self.buffer)
        if text.endswith('\n'):
            self.lines.insert(row + 1, MyString(text[:-1]))
            self.modified = True
            return row + 1, 0
        at = min(column + 1, len(self.lines[row]))
        self.lines[row].insert(at, self.buffer)
        self.modified = True
        return row, at + len(text)

    

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
        self.sequence = ""
        self.count = ""
        self.search_direction = "/"
        self.search_origin = (0, 0)
        self.search_match = None
        self.last_search = ""
        self.last_direction = "/"
        self.message = ("i: insert | :h: help | :w: save | :q: quit"
                        " | :o: open | / ?: search")
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
            if name == "o" and argument:
                if self.document.modified:
                    self.message = "Unsaved changes; use :w or :o! to discard"
                else:
                    self.open_file(argument)
            elif name == "x" and argument is None:
                if self.document.path is None:
                    self.running = False
                else:
                    self.document.save(self.document.path)
                    self.running = False
            elif name in {"w", "wq"}:
                self.document.save(Path(argument) if argument else None)
                self.message = "Saved"
                if name == "wq":
                    self.running = False
            elif name in {"q", "q!"} and argument is None:
                if self.document.modified and name != "q!":
                    self.message = "Unsaved changes; use :w or :q!"
                else:
                    self.running = False
            elif name == "o!" and argument:
                self.open_file(argument)
            elif name in {"h", "help"} and argument is None:
                self.show_help()
            elif name.isdigit():
                self.column = 0
                self.row = int(name)
            elif name:
                self.message = "Unknown command: " + self.command
        except (OSError, ValueError) as error:
            self.message = str(error)
        self.command = ""
        self.mode = "NORMAL"
        self.clamp()

    def show_help(self) -> None:
        try:
            self.document = Document.open(HELP_PATH)
        except (OSError, ValueError) as error:
            self.message = "Cannot open help: " + str(error)
            return
        self.row = self.column = 0
        self.preferred_column = 0
        self.pending = self.sequence = self.count = ""
        self.search_match = None
        self.last_search = ""
        self.message = "Help: " + str(HELP_PATH)

    def open_file(self, name: str) -> None:
        self.document = Document.open(Path(name))
        self.row = self.column = 0
        self.preferred_column = 0
        self.search_match = None
        self.last_search = ""
        path = str(self.document.path) if self.document.path else "[New file]"
        self.message = "Opened " + path

    def find_match(self, pattern: str, origin: tuple[int, int]) -> tuple[int, int] | None:
        forward = self.search_direction == "/"
        start_row, start_column = origin
        if forward:
            for row in range(start_row, len(self.document.lines)):
                text = str(self.document.lines[row])
                column = text.find(pattern, start_column + 1 if row == start_row else 0)
                if column != -1:
                    return row, column
        else:
            for row in range(start_row, -1, -1):
                text = str(self.document.lines[row])
                column = text.rfind(pattern, 0, start_column if row == start_row else len(text))
                if column != -1:
                    return row, column
        return None

    def run_search(self) -> None:
        if not self.command:
            self.search_match = None
            return
        found = self.find_match(self.command, self.search_origin)
        if found is None:
            self.search_match = None
            self.message = "Pattern not found: " + self.command
        else:
            self.row, self.column = found
            self.preferred_column = self.column
            self.search_match = (self.row, self.column, len(self.command))
            self.message = f"{self.search_direction}{self.command}"
        self.clamp()

    def execute_search(self) -> None:
        pattern = self.command
        self.run_search()
        self.command = ""
        if self.search_match is not None:
            self.last_search = pattern
            self.last_direction = self.search_direction
            self.message = ""
        self.mode = "NORMAL"

    def repeat_search(self, reverse: bool = False) -> None:
        if not self.last_search:
            self.message = "No previous search"
            return
        self.search_direction = "?" if ((self.last_direction == "/") == reverse) else "/"
        found = self.find_match(self.last_search, (self.row, self.column))
        if found is None:
            self.search_match = None
            self.message = "Pattern not found: " + self.last_search
        else:
            self.row, self.column = found
            self.preferred_column = self.column
            self.search_match = (self.row, self.column, len(self.last_search))
            self.message = ""
        self.clamp()

    def handle_key(self, key: str) -> None:
        last_space = self.document.lines[self.row].rfind(" ")
        first_space = self.document.lines[self.row].find(" ")
        if key == "ESC":
            self.mode = "NORMAL"
            self.pending = self.command = self.count = self.sequence = ""
            self.search_match = None
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

        if self.mode == "SEARCH":
            if key == "ENTER":
                self.execute_search()
            elif key == "BACKSPACE":
                self.command = self.command[:-1]
                self.run_search()
            elif len(key) == 1 and key.isprintable():
                self.command += key
                self.run_search()
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
                self.preferred_column = 0
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
            if pending == "r" and len(key) == 1 and " " <= key <= "~":
                self.document.delete_character(self.row, self.column)
                self.document.insert(self.row, self.column, key)
                # self.mode = "INSERT"
                self.sequence = ""
                self.clamp()
                self.preferred_column = self.column
                return
            count, self.count = self.count, ""
            sequence = self.sequence + key
            if sequence == "diw":
                self.sequence = ""
                self.document.delete_word(self.row, self.column)
            elif sequence == "di":
                self.sequence = "di"
            elif len(key) == 1 and key.isdigit() and (key != "0" or count):
                self.sequence = ""
                self.count = count + key
            else:
                if sequence not in ("d", "i", "di"):
                    self.sequence = ""
                self.dispatch_normal(key, pending, count, first_space, last_space,
                                     sequence)
        self.clamp()
        self.preferred_column = self.column

    def dispatch_normal(self, key, pending, count, first_space, last_space, sequence):
        if key == "0":
            self.column = 0
        elif key == "$":
            self.column = len(self.document.lines[self.row])
        elif key == "w":
            if pending == "y":
                self.document.copy_word(self.row, self.column)
                self.sequence = ""
            else:
                next_space = self.document.lines[self.row].find(" ", self.column+1)
                if(next_space == self.column+1):
                    next_space = self.document.lines[self.row].find(" ", self.column+2)
                if(self.column+1 >= last_space):
                    self.column = len(self.document.lines[self.row])
                else:
                    self.column = next_space - 1
        elif key == "b":
            near_space = self.document.lines[self.row].rfind(" ", 0, self.column-1)
            if(self.column-1 <= first_space):
                self.column = 0
            else:
                if(near_space == first_space):
                    self.column = first_space + 1
                else:
                    self.column = near_space + 1
        elif key == "g":
            if pending == "g":
                self.row, self.column = 0, 0
            else:
                self.pending = "g"
        elif key == "G":
            if count:
                self.row = min(int(count), len(self.document.lines)) - 1
            else:
                self.row = len(self.document.lines) - 1
            self.column = 0
        elif key == "PG_UP":
            wd, ht = shutil.get_terminal_size((80, 24))
            screen_ht = max(1, ht - 2)
            self.row = max(0, self.row - screen_ht)
            self.column = 0
        elif key == "PG_DN":
            wd, ht = shutil.get_terminal_size((80, 24))
            screen_ht = max(1, ht - 2)
            self.row = min(len(self.document.lines) - 1, self.row + screen_ht)
            self.column = 0
        elif key == "x":
            self.document.delete_character(self.row, self.column)
        elif key == "r":
            self.pending = "r"
            self.sequence = "r"
        elif key == "i":
            self.mode = "INSERT"
            self.sequence = ""
        elif key == "I":
            self.column = 0
            self.mode = "INSERT"
            self.sequence = ""
        elif key == "A":
            self.column = len(self.document.lines[self.row])
            self.mode = "INSERT"
            self.sequence = ""
        elif key == "S":
            self.document.delete_line(self.row)
            self.mode = "INSERT"
            self.sequence = ""
        elif key == "d":
            if pending == "d":
                self.document.delete_line(self.row)
                self.sequence = ""
            else:
                self.pending = "d"
                self.sequence = "d"
        elif key == "y":
            if pending == "y":
                self.document.copy_line(self.row)
                self.sequence = ""
            else:
                self.pending = "y"
                self.sequence = "y"
        elif key == "w":
            if pending == "y":
                self.document.copy_word(self.row, self.column)
                self.sequence = ""
            else:
                self.pending = "w"
                self.sequence = "w"
        elif key == "p":
            self.row, self.column = self.document.paste(self.row, self.column)
            self.preferred_column = self.column
        elif key == "n":
            self.repeat_search()
        elif key == "N":
            self.repeat_search(reverse=True)
        elif key == ":":
            self.mode = "COMMAND"
            self.command = ""
            self.sequence = ""
        elif key in {"/", "?"}:
            self.mode = "SEARCH"
            self.search_direction = key
            self.search_origin = (self.row, self.column)
            self.command = ""
            self.sequence = ""


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
                    "S": "DELETE", "I" : "PG_UP", "Q" : "PG_DN"}.get(msvcrt.getwch(), "UNKNOWN")
        return {"\x1b": "ESC", "\r": "ENTER", "\b": "BACKSPACE",
                "\t": "TAB", "\x03": "ESC", "\x04": "PG_DN", "\x15": "PG_UP"}.get(key, key)


class TerminalView:
    def __init__(self) -> None:
        self.top = self.left = 0

    def highlight_match(self, editor: Editor, row: int, visible: str) -> str:
        match = editor.search_match
        if match is None or match[0] != row:
            return visible
        _, column, length = match
        raw = str(editor.document.lines[row])
        start = len(raw[:column].expandtabs(4)) - self.left
        end = len(raw[:column + length].expandtabs(4)) - self.left
        if length and start < len(visible) and end > 0:
            start = max(0, start)
            end = min(len(visible), end)
            return (visible[:start] + "\x1b[7m" + visible[start:end]
                    + "\x1b[0m" + visible[end:])
        return visible

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
            visible = text[self.left:self.left + width]
            output.append(f"\x1b[{screen_row + 1};1H\x1b[2K"
                          + self.highlight_match(editor, row, visible))

        path = str(editor.document.path) if editor.document.path else "[New file]"

        status = (f"{editor.mode} | {path} {'[+]' if editor.document.modified else ''}"
                  f" | {editor.row + 1}:{editor.column + 1} {editor.sequence}")
        
        output.append(f"\x1b[{body_height + 1};1H\x1b[2K\x1b[7m"
                      + safe_screen_text(status)[:width].ljust(width) + "\x1b[0m")
        if editor.mode == "COMMAND":
            message = ":" + editor.command
        elif editor.mode == "SEARCH":
            message = editor.search_direction + editor.command
        else:
            message = editor.message

        output.append(f"\x1b[{body_height + 2};1H\x1b[2K" + safe_screen_text(message)[-width:])

        if editor.mode in {"COMMAND", "SEARCH"}:
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