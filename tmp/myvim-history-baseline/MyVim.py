
from pathlib import Path
import os
import re
import shutil
import sys
import tempfile
from abc import ABC, abstractmethod

from my_string import MyString

try:
    import curses
except ImportError:
    curses = None

try:
    from pygments.lexers import get_lexer_by_name
    from pygments.token import Token
except ImportError:
    get_lexer_by_name = Token = None


HELP_PATH = Path(__file__).with_name("help.txt")


def validate_text(text: str) -> None:
    if any(ch not in "\n\t" and not " " <= ch <= "~" for ch in text):
        raise ValueError("Only printable ASCII, tabs and line breaks are supported")


class Command(ABC):
    """An editor action. Each instance stores its own state for undo."""

    @abstractmethod
    def execute(self, editor: "Editor") -> None:
        pass

    @abstractmethod
    def undo(self, editor: "Editor") -> None:
        pass

    def _remember(self, editor: "Editor") -> None:
        self._editor_state = {
            key: value for key, value in vars(editor).items()
            if key not in {"document", "history"}
        }
        self._document = editor.document
        self._document_state = (
            editor.document.text(), editor.document.path,
            editor.document.newline, editor.document.modified,
            str(editor.document.buffer),
        )

    def _finish(self, editor: "Editor") -> None:
        editor.clamp()
        editor.preferred_column = editor.column

    def _restore(self, editor: "Editor") -> None:
        if not hasattr(self, "_editor_state"):
            return
        text, path, newline, modified, buffer = self._document_state
        editor.document = self._document
        editor.document.lines = [MyString(line) for line in text.split("\n")]
        editor.document.path = path
        editor.document.newline = newline
        editor.document.modified = modified
        editor.document.buffer = MyString(buffer)
        vars(editor).update(self._editor_state)





class EscapeCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.mode = "NORMAL"
        editor.pending = editor.command = editor.count = editor.sequence = ""
        editor.search_match = None
        editor.clamp()
        editor.preferred_column = editor.column

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class MoveLeftCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.pending = ""
        editor.row += 0
        editor.column = editor.preferred_column if 0 else editor.column + -1
        editor.clamp()
        if not 0:
            editor.preferred_column = editor.column

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class MoveRightCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.pending = ""
        editor.row += 0
        editor.column = editor.preferred_column if 0 else editor.column + 1
        editor.clamp()
        if not 0:
            editor.preferred_column = editor.column

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class MoveUpCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.pending = ""
        editor.row += -1
        editor.column = editor.preferred_column if -1 else editor.column + 0
        editor.clamp()
        if not -1:
            editor.preferred_column = editor.column

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class MoveDownCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.pending = ""
        editor.row += 1
        editor.column = editor.preferred_column if 1 else editor.column + 0
        editor.clamp()
        if not 1:
            editor.preferred_column = editor.column

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class LineStartCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.column = 0
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class LineEndCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.column = len(editor.document.lines[editor.row])
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class NextWordCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        line = editor.document.lines[editor.row]
        next_space = line.find(" ", editor.column + 1)
        if next_space == editor.column + 1:
            next_space = line.find(" ", editor.column + 2)
        if editor.column + 1 >= line.rfind(" "):
            editor.column = len(line)
        else:
            editor.column = next_space - 1
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class PreviousWordCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        line = editor.document.lines[editor.row]
        first_space = line.find(" ")
        near_space = line.rfind(" ", 0, editor.column - 1)
        if editor.column - 1 <= first_space:
            editor.column = 0
        else:
            editor.column = first_space + 1 if near_space == first_space else near_space + 1
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class FirstLineCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.row = editor.column = 0
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class LastLineCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.row = len(editor.document.lines) - 1
        editor.column = 0
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class GoToLineCommand(Command):
    def __init__(self, number: int) -> None:
        self.number = number

    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.row = min(self.number, len(editor.document.lines)) - 1
        editor.column = 0
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class PageUpCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        height = (editor.view.screen.getmaxyx()[0]
                  if editor.view is not None and editor.view.screen is not None
                  else shutil.get_terminal_size((80, 24))[1])
        editor.row += -1 * max(1, height - 2)
        editor.column = 0
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class PageDownCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        height = (editor.view.screen.getmaxyx()[0]
                  if editor.view is not None and editor.view.screen is not None
                  else shutil.get_terminal_size((80, 24))[1])
        editor.row += 1 * max(1, height - 2)
        editor.column = 0
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class DeleteCharacterCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        line = editor.document.lines[editor.row]
        if editor.column < len(line):
            line.erase(editor.column, 1)
            editor.document.modified = True
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class ReplaceCharacterCommand(Command):
    def __init__(self, text: str) -> None:
        self.text = text

    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        validate_text(self.text)
        if "\n" in self.text:
            raise ValueError("Use SplitLineCommand for line breaks")
        line = editor.document.lines[editor.row]
        if editor.column < len(line):
            line.erase(editor.column, 1)
            editor.document.modified = True
        if self.text:
            editor.document.lines[editor.row].insert(editor.column, self.text)
            editor.document.modified = True
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class DeleteLineCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        if len(editor.document.lines) == 1:
            if not editor.document.lines[0].empty():
                editor.document.lines[0] = MyString("")
                editor.document.modified = True
        else:
            del editor.document.lines[editor.row]
            editor.document.modified = True
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class DeleteWordCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        text = str(editor.document.lines[editor.row])
        end = editor.column
        while end < len(text) and not text[end].isspace():
            end += 1
        while end < len(text) and text[end].isspace():
            end += 1
        if end > editor.column:
            editor.document.lines[editor.row].erase(editor.column, end - editor.column)
            editor.document.modified = True
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class DeleteInnerWordCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        text = str(editor.document.lines[editor.row])
        if editor.column < len(text):
            start = editor.column
            end = editor.column
            while start > 0 and not text[start - 1].isspace():
                start -= 1
            while end < len(text) and not text[end].isspace():
                end += 1
            if end > start:
                editor.document.lines[editor.row].erase(start, end - start)
                editor.document.modified = True
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class CopyLineCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.document.buffer = MyString(str(editor.document.lines[editor.row]))
        editor.document.buffer.append("\n")
        editor.document.modified = True
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class CopyWordCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        line = editor.document.lines[editor.row]
        if editor.column < len(line):
            text = str(line)
            start = text.rfind(" ", 0, editor.column) + 1
            end = text.find(" ", editor.column)
            if end == -1:
                end = len(text)
            editor.document.buffer = MyString(text[start:end])
            editor.document.modified = True
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class PasteCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        text = str(editor.document.buffer)
        if text.endswith("\n"):
            editor.document.lines.insert(editor.row + 1, MyString(text[:-1]))
            editor.row += 1
            editor.column = 0
        else:
            at = min(editor.column + 1, len(editor.document.lines[editor.row]))
            editor.document.lines[editor.row].insert(at, editor.document.buffer)
            editor.column = at + len(text)
        editor.document.modified = True
        editor.preferred_column = editor.column
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class InsertModeCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.mode = "INSERT"
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class InsertAtStartCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.column = 0
        editor.mode = "INSERT"
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class AppendCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.column = len(editor.document.lines[editor.row])
        editor.mode = "INSERT"
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class SubstituteLineCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.document.lines[editor.row] = MyString("")
        editor.document.modified = True
        editor.column = 0
        editor.mode = "INSERT"
        editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class SplitLineCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        text = str(editor.document.lines[editor.row])
        editor.document.lines[editor.row] = MyString(text[:editor.column])
        editor.document.lines.insert(editor.row + 1, MyString(text[editor.column:]))
        editor.document.modified = True
        editor.row += 1
        editor.column = editor.preferred_column = 0
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class BackspaceCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        if editor.column:
            editor.document.lines[editor.row].erase(editor.column - 1, 1)
            editor.document.modified = True
            editor.column -= 1
        elif editor.row:
            previous_length = len(editor.document.lines[editor.row - 1])
            editor.document.lines[editor.row - 1].append(
                str(editor.document.lines[editor.row]))
            del editor.document.lines[editor.row]
            editor.document.modified = True
            editor.row -= 1
            editor.column = previous_length
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class InsertTextCommand(Command):
    def __init__(self, text: str) -> None:
        self.text = text

    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        validate_text(self.text)
        if "\n" in self.text:
            raise ValueError("Use SplitLineCommand for line breaks")
        if self.text:
            editor.document.lines[editor.row].insert(editor.column, self.text)
            editor.document.modified = True
        editor.column += len(self.text)
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class TabCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        InsertTextCommand("    ").execute(editor)
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class CommandModeCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.mode = "COMMAND"
        editor.command = editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class ForwardSearchCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.mode = "SEARCH"
        editor.search_direction = "/"
        editor.search_origin = (editor.row, editor.column)
        editor.command = editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


    @staticmethod
    def find_match(document: "Document", pattern: str,
                   origin: tuple[int, int]) -> tuple[int, int] | None:
        start_row, start_column = origin
        for row in range(start_row, len(document.lines)):
            text = str(document.lines[row])
            column = text.find(pattern, start_column + 1 if row == start_row else 0)
            if column != -1:
                return row, column
        return None


class BackwardSearchCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.mode = "SEARCH"
        editor.search_direction = "?"
        editor.search_origin = (editor.row, editor.column)
        editor.command = editor.sequence = ""
        self._finish(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


    @staticmethod
    def find_match(document: "Document", pattern: str,
                   origin: tuple[int, int]) -> tuple[int, int] | None:
        start_row, start_column = origin
        for row in range(start_row, -1, -1):
            text = str(document.lines[row])
            column = text.rfind(pattern, 0, start_column if row == start_row else len(text))
            if column != -1:
                return row, column
        return None


class SearchPreviewCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        if not editor.command:
            editor.search_match = None
            return
        found = editor.find_match(editor.command, editor.search_origin)
        if found is None:
            editor.search_match = None
            editor.message = "Pattern not found: " + editor.command
        else:
            editor.row, editor.column = found
            editor.preferred_column = editor.column
            editor.search_match = (editor.row, editor.column, len(editor.command))
            editor.message = f"{editor.search_direction}{editor.command}"
        editor.clamp()

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class ConfirmSearchCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        pattern = editor.command
        editor.run_search()
        editor.command = ""
        if editor.search_match is not None:
            editor.last_search = pattern
            editor.last_direction = editor.search_direction
            editor.message = ""
        editor.mode = "NORMAL"

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class RepeatSearchCommand(Command):
    def __init__(self, reverse: bool = False) -> None:
        self.reverse = reverse

    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        if not editor.last_search:
            editor.message = "No previous search"
            return
        editor.search_direction = "?" if ((editor.last_direction == "/") == self.reverse) else "/"
        found = editor.find_match(editor.last_search, (editor.row, editor.column))
        if found is None:
            editor.search_match = None
            editor.message = "Pattern not found: " + editor.last_search
        else:
            editor.row, editor.column = found
            editor.preferred_column = editor.column
            editor.search_match = (editor.row, editor.column, len(editor.last_search))
            editor.message = ""
        editor.clamp()

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class ReverseRepeatSearchCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        RepeatSearchCommand(reverse=True).execute(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class OpenFileCommand(Command):
    def __init__(self, name: str) -> None:
        self.name = name

    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        if editor.document.modified:
            editor.message = "Unsaved changes; use :w or :o! to discard"
        else:
            self._open(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)

    def _open(self, editor: "Editor") -> None:
        editor.document = Document.open_file(Path(self.name))
        editor.row = editor.column = editor.preferred_column = 0
        editor.search_match = None
        editor.last_search = ""
        path = str(editor.document.path) if editor.document.path else "[New file]"
        editor.message = "Opened " + path


class ForceOpenFileCommand(Command):
    def __init__(self, name: str) -> None:
        self.name = name

    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        OpenFileCommand(self.name)._open(editor)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class HelpCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        try:
            editor.document = Document.open_file(HELP_PATH)
        except (OSError, ValueError) as error:
            editor.message = "Cannot open help: " + str(error)
            return
        editor.row = editor.column = editor.preferred_column = 0
        editor.pending = editor.sequence = editor.count = ""
        editor.search_match = None
        editor.last_search = ""
        editor.message = "Help: " + str(HELP_PATH)

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class WriteCommand(Command):
    def __init__(self, path: Path | None = None) -> None:
        self.path = path

    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        self._target = self.path.resolve() if self.path is not None else editor.document.path
        self._written = False
        if self._target is None:
            raise ValueError("No file name; use :w path")
        self._previous_bytes = self._target.read_bytes() if self._target.exists() else None
        editor.document.save(self.path)
        self._written = True
        editor.message = "Saved"

    def undo(self, editor: "Editor") -> None:
        if getattr(self, "_written", False):
            if self._previous_bytes is None:
                self._target.unlink(missing_ok=True)
            else:
                self._target.write_bytes(self._previous_bytes)
            self._written = False
        self._restore(editor)


class WriteQuitCommand(Command):
    def __init__(self, path: Path | None = None) -> None:
        self.path = path

    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        self._write = WriteCommand(self.path)
        self._write.execute(editor)
        editor.running = False

    def undo(self, editor: "Editor") -> None:
        self._write.undo(editor)
        self._restore(editor)


class ExitCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        self._write = None
        if editor.document.path is not None:
            self._write = WriteCommand()
            self._write.execute(editor)
        editor.running = False

    def undo(self, editor: "Editor") -> None:
        if self._write is not None:
            self._write.undo(editor)
        self._restore(editor)


class QuitCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        if editor.document.modified:
            editor.message = "Unsaved changes; use :w or :q!"
        else:
            editor.running = False

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class ForceQuitCommand(Command):
    def execute(self, editor: "Editor") -> None:
        self._remember(editor)
        editor.running = False

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)

class ShowNumCommand(Command):
    def execute(self, editor: "Editor") -> None:
        if editor.view is None: raise ValueError("Error connecting TerminalView")
        self._remember(editor)
        self._view = editor.view
        self._prev = self._view.show_num 
        self._view.show_num = not self._view.show_num
    
    def undo(self, editor: "Editor") -> None:
        self._view.show_num = self._prev
        self._restore(editor)

class SyntaxCommand(Command):
    def execute(self, editor: "Editor") -> None:
        if not editor.syntax_enabled and get_lexer_by_name is None:
            raise ValueError("Install syntax support: python -m pip install Pygments")
        self._remember(editor)
        editor.syntax_enabled = not editor.syntax_enabled
        editor.message = "Syntax highlighting " + ("on" if editor.syntax_enabled else "off")

    def undo(self, editor: "Editor") -> None:
        self._restore(editor)


class HighlightingDecorator():
    def __init__(self, source: str):
        self.source = source
        self.comment_starts = {"#", "//"}
        # Statement / Conditional / Repeat / Operator / Exception: yellow.
        self.statements = {
            # Python
            'Ellipsis', 'False', 'None', 'NotImplemented', 'True', '__debug__', 'and', 'as',
            'assert', 'async', 'await', 'break', 'case', 'class', 'continue', 'def', 'del', 'elif',
            'else', 'except', 'finally', 'for', 'global', 'if', 'in', 'is', 'lambda', 'match',
            'nonlocal', 'not', 'or', 'pass', 'raise', 'return', 'try', 'type', 'while', 'with',
            'yield',
            # Java additions
            'AbstractMethodError', 'ArithmeticException', 'ArrayIndexOutOfBoundsException',
            'ArrayStoreException', 'AssertionError', 'BootstrapMethodError', 'ClassCastException',
            'ClassCircularityError', 'ClassFormatError', 'ClassNotFoundException',
            'CloneNotSupportedException', 'EnumConstantNotPresentException', 'Error', 'Exception',
            'ExceptionInInitializerError', 'IllegalAccessError', 'IllegalAccessException',
            'IllegalArgumentException', 'IllegalCallerException', 'IllegalMonitorStateException',
            'IllegalStateException', 'IllegalThreadStateException', 'IncompatibleClassChangeError',
            'IndexOutOfBoundsException', 'InstantiationError', 'InstantiationException',
            'InternalError', 'InterruptedException', 'LayerInstantiationException', 'LinkageError',
            'MatchException', 'NegativeArraySizeException', 'NoClassDefFoundError',
            'NoSuchFieldError', 'NoSuchFieldException', 'NoSuchMethodError',
            'NoSuchMethodException', 'NullPointerException', 'NumberFormatException',
            'OutOfMemoryError', 'ReflectiveOperationException', 'RuntimeException',
            'SecurityException', 'StackOverflowError', 'StringIndexOutOfBoundsException',
            'ThreadDeath', 'Throwable', 'TypeNotPresentException', 'UnknownError',
            'UnsatisfiedLinkError', 'UnsupportedClassVersionError',
            'UnsupportedOperationException', 'VerifyError', 'VirtualMachineError',
            'WrongThreadException', 'catch', 'default', 'do', 'exports', 'instanceof', 'module',
            'new', 'open', 'opens', 'provides', 'requires', 'switch', 'throw', 'uses', 'var',
            'when',
            # C# additions
            '!', '!=', '&&', '*', '*=', '+', '++', '+=', '-', '--', '-=', '/', '/=', ':', '<',
            '<<', '<=', '=', '==', '=>', '>', '>=', '>>', '?', '^', 'a', 'add', 'ascending', 'b',
            'base', 'br', 'by', 'c', 'checked', 'code', 'cref', 'descending', 'equals', 'example',
            'exception', 'explicit', 'fixed', 'foreach', 'from', 'get', 'goto', 'group', 'href',
            'i', 'implicit', 'include', 'inheritdoc', 'init', 'into', 'join', 'let', 'list',
            'lock', 'nameof', 'on', 'orderby', 'out', 'para', 'param', 'paramref', 'params', 'ref',
            'remarks', 'remove', 'returns', 'see', 'seealso', 'select', 'set', 'sizeof',
            'stackalloc', 'summary', 'this', 'typeof', 'typeparam', 'typeparamref', 'u',
            'unchecked', 'unsafe', 'using', 'value', 'where', '||',
            # C / C++ additions (standard-library and GNU types included)
            '_Alignof', '_Generic', '_Static_assert', '__alignof__', '__asm__', '__imag__',
            '__real__', '__typeof__', 'alignof', 'asm', 'static_assert', 'typeof_unqual',
            # C++ additions
            '^^', 'and_eq', 'bitand', 'bitor', 'co_await', 'co_return', 'co_yield', 'compl',
            'const_cast', 'const_pointer_cast', 'contract_assert', 'delete', 'dynamic_cast',
            'dynamic_pointer_cast', 'friend', 'noexcept', 'not_eq', 'operator', 'or_eq', 'private',
            'protected', 'public', 'reinterpret_cast', 'reinterpret_pointer_cast', 'static_cast',
            'static_pointer_cast', 'typeid', 'xor', 'xor_eq',
        }

        # Type / StorageClass / Structure / Typedef: light green.
        self.classes = {
            # Python
            'ArithmeticError', 'AssertionError', 'AttributeError', 'BaseException',
            'BaseExceptionGroup', 'BlockingIOError', 'BrokenPipeError', 'BufferError',
            'BytesWarning', 'ChildProcessError', 'ConnectionAbortedError', 'ConnectionError',
            'ConnectionRefusedError', 'ConnectionResetError', 'DeprecationWarning', 'EOFError',
            'EncodingWarning', 'EnvironmentError', 'Exception', 'ExceptionGroup',
            'FileExistsError', 'FileNotFoundError', 'FloatingPointError', 'FutureWarning',
            'GeneratorExit', 'IOError', 'ImportError', 'ImportWarning', 'IndentationError',
            'IndexError', 'InterruptedError', 'IsADirectoryError', 'KeyError', 'KeyboardInterrupt',
            'LookupError', 'MemoryError', 'ModuleNotFoundError', 'NameError', 'NotADirectoryError',
            'NotImplementedError', 'OSError', 'OverflowError', 'PendingDeprecationWarning',
            'PermissionError', 'ProcessLookupError', 'PythonFinalizationError', 'RecursionError',
            'ReferenceError', 'ResourceWarning', 'RuntimeError', 'RuntimeWarning',
            'StopAsyncIteration', 'StopIteration', 'SyntaxError', 'SyntaxWarning', 'SystemError',
            'SystemExit', 'TabError', 'TimeoutError', 'TypeError', 'UnboundLocalError',
            'UnicodeDecodeError', 'UnicodeEncodeError', 'UnicodeError', 'UnicodeTranslateError',
            'UnicodeWarning', 'UserWarning', 'ValueError', 'Warning', 'WindowsError',
            'ZeroDivisionError',
            # Java additions
            'Appendable', 'AutoCloseable', 'CharSequence', 'Cloneable', 'Deprecated',
            'FunctionalInterface', 'Override', 'ProcessHandle', 'Readable', 'Runnable',
            'SafeVarargs', 'SuppressWarnings', 'abstract', 'boolean', 'byte', 'char', 'class',
            'double', 'enum', 'extends', 'final', 'float', 'implements', 'int', 'interface',
            '@interface', 'default', 'long', 'module', 'non-sealed', 'permits', 'private', 'protected', 'public',
            'record', 'sealed', 'short', 'static', 'strictfp', 'super', 'synchronized', 'this',
            'throws', 'transient', 'transitive', 'void', 'volatile',
            # C# additions
            'T', 'alias', 'async', 'bool', 'checked', 'const', 'decimal', 'delegate', 'dynamic',
            'event', 'extern', 'fixed', 'global', 'internal', 'managed', 'namespace', 'nint',
            'nuint', 'object', 'operator', 'override', 'partial', 'readonly', 'sbyte', 'string',
            'struct', 'uint', 'ulong', 'unmanaged', 'unsafe', 'ushort', 'var', 'virtual', 'where',
            # C / C++ additions (standard-library and GNU types included)
            'DIR', 'FILE', '_Alignas', '_Atomic', '_BitInt', '_Bool', '_Complex', '_Decimal128',
            '_Decimal32', '_Decimal64', '_Imaginary', '_Noreturn', '_Thread_local',
            '__attribute__', '__complex__', '__const__', '__extension__', '__inline', '__inline__',
            '__label__', '__noreturn__', '__restrict__', '__volatile__', 'alignas', 'atomic_bool',
            'atomic_char', 'atomic_char16_t', 'atomic_char32_t', 'atomic_int_fast16_t',
            'atomic_int_fast32_t', 'atomic_int_fast64_t', 'atomic_int_fast8_t',
            'atomic_int_least16_t', 'atomic_int_least32_t', 'atomic_int_least64_t',
            'atomic_int_least8_t', 'atomic_intmax_t', 'atomic_intptr_t', 'atomic_llong',
            'atomic_long', 'atomic_ptrdiff_t', 'atomic_schar', 'atomic_size_t', 'atomic_uchar',
            'atomic_uint_fast16_t', 'atomic_uint_fast32_t', 'atomic_uint_fast64_t',
            'atomic_uint_fast8_t', 'atomic_uint_least16_t', 'atomic_uint_least32_t',
            'atomic_uint_least64_t', 'atomic_uint_least8_t', 'atomic_uintmax_t',
            'atomic_uintptr_t', 'atomic_ullong', 'atomic_ulong', 'atomic_wchar_t', 'auto',
            'char16_t', 'char32_t', 'char8_t', 'clock_t', 'complex', 'constexpr', 'deprecated',
            'div_t', 'fallthrough', 'fpos_t', 'imaginary', 'inline', 'int16_t', 'int32_t',
            'int64_t', 'int8_t', 'int_fast16_t', 'int_fast32_t', 'int_fast64_t', 'int_fast8_t',
            'int_least16_t', 'int_least32_t', 'int_least64_t', 'int_least8_t', 'intmax_t',
            'intptr_t', 'jmp_buf', 'ldiv_t', 'max_align_t', 'maybe_unused', 'mbstate_t',
            'nodiscard', 'noreturn', 'nullptr_t', 'off_t', 'ptrdiff_t', 'register', 'reproducible',
            'restrict', 'sig_atomic_t', 'signed', 'size_t', 'ssize_t', 'thread_local', 'time_t',
            'typedef', 'u_int16_t', 'u_int32_t', 'u_int64_t', 'u_int8_t', 'uint16_t', 'uint32_t',
            'uint64_t', 'uint8_t', 'uint_fast16_t', 'uint_fast32_t', 'uint_fast64_t',
            'uint_fast8_t', 'uint_least16_t', 'uint_least32_t', 'uint_least64_t', 'uint_least8_t',
            'uintmax_t', 'uintptr_t', 'union', 'unsequenced', 'unsigned', 'va_list', 'wchar_t',
            'wctrans_t', 'wctype_t', 'wint_t',
            # C++ additions
            'bfloat16_t', 'concept', 'consteval', 'constinit', 'decltype', 'explicit', 'export',
            'float128_t', 'float16_t', 'float32_t', 'float64_t', 'mutable', 'template', 'typename',
        }

        # PreProc / Include / Define / Macro / PreCondit: light blue.
        self.preproc = {
            # Python
            '@', 'from', 'import', 'lazy',
            # Java additions
            'native', 'package', 'static', 'to', 'with',
            # C# additions
            '#!', '#define', '#elif', '#else', '#endif', '#endregion', '#error', '#if', '#line',
            '#load', '#nullable', '#pragma', '#r', '#region', '#undef', '#warning', 'define',
            'elif', 'else', 'endif', 'endregion', 'error', 'global', 'if', 'line', 'load',
            'nullable', 'pragma', 'r', 'region', 'undef', 'warning',
            # C / C++ additions (standard-library and GNU types included)
            '#', '#elifdef', '#elifndef', '#embed', '#ifdef', '#ifndef', '#include', '#warn', '%:',
            '%:define', '%:elif', '%:elifdef', '%:elifndef', '%:else', '%:embed', '%:endif',
            '%:error', '%:if', '%:ifdef', '%:ifndef', '%:include', '%:line', '%:pragma', '%:undef',
            '%:warn', '%:warning', 'defined', 'elifdef', 'elifndef', 'embed', 'ifdef', 'ifndef', 'include',
            'warn',
            # C++ additions
            'export', 'module',
        }

        # Special / SpecialChar / Delimiter: light red.
        self.specchars = {
            # Python
            '!a', '!r', '!s', ':', '=', '>>>', '\\\n', '\\"', "\\'", '\\0', '\\1', '\\2', '\\3',
            '\\4', '\\5', '\\6', '\\7', '\\N{', '\\U', '\\\\', '\\a', '\\b', '\\f', '\\n', '\\r',
            '\\t', '\\u', '\\v', '\\x', '{', '}',
            # Java additions
            '@author', '@deprecated', '@exception', '@hidden', '@param',
            '@provides', '@return', '@see', '@serial', '@serialData', '@serialField', '@since',
            '@spec', '@throws', '@uses', '@version', '\\s', '{@code', '{@docRoot', '{@index',
            '{@inheritDoc', '{@link', '{@linkplain', '{@literal', '{@snippet', '{@summary',
            '{@systemProperty', '{@value',
            # C# additions
            '""', ',',
            # C / C++ additions (standard-library and GNU types included)
            '%%', '%lA', '%lE', '%lF', '%lG', '%la', '%le', '%lf', '%lg', '%A', '%C', '%D', '%E',
            '%F', '%G', '%LA', '%LE', '%LF', '%LG', '%La', '%Le',
            '%Lf', '%Lg', '%O', '%S', '%U', '%X', '%[', '%[^', '%a', '%b', '%c', '%d', '%e', '%f',
            '%g', '%hX', '%hb', '%hd', '%hhX', '%hhb', '%hhd', '%hhi', '%hhn', '%hho', '%hhu',
            '%hhx', '%hi', '%hn', '%ho', '%hu', '%hx', '%i', '%jX', '%jb', '%jd', '%ji', '%jn',
            '%jo', '%ju', '%jx', '%lX', '%lb', '%lc', '%ld', '%li', '%llX', '%llb', '%lld', '%lli',
            '%lln', '%llo', '%llu', '%llx', '%ln', '%lo', '%ls', '%lu', '%lx', '%n', '%o', '%p',
            '%s', '%tX', '%tb', '%td', '%ti', '%tn', '%to', '%tu', '%tx', '%u', '%x', '%zX', '%zb',
            '%zd', '%zi', '%zn', '%zo', '%zu', '%zx', '\\?', '\\e',
            # C++ additions
            ':]', 'LR"', 'R"', 'UR"', '[:', '[[', ']]', 'u8R"', 'uR"',
        }
        


        

    LANGUAGES = {
        ".py": "python", ".pyw": "python",
        ".c": "c", ".h": "c",
        ".cpp": "cpp", ".cc": "cpp", ".cxx": "cpp",
        ".hpp": "cpp", ".hh": "cpp", ".hxx": "cpp",
        ".cs": "csharp", ".java": "java",
    }
    ESCAPE = re.compile(r"\\(?:[0-7]{1,3}|x[0-9a-fA-F]{1,4}|u[0-9a-fA-F]{4}|U[0-9a-fA-F]{8}|N\{[^}]+\}|[^\n])")
    FORMAT = re.compile(
        r"%(?:%|(?:\d+\$)?[-+ #0']*(?:\d+|\*(?:\d+\$)?)?"
        r"(?:\.(?:\d+|\*(?:\d+\$)?))?(?:hh|ll|[hlLjzt])?[aAbdiuoxXDOUfFeEgGcCsSpn])"
    )

    def language(self, path: Path | None) -> str | None:
        return self.LANGUAGES.get(path.suffix.lower()) if path else "python"

    def _category(self, token, value: str, language: str) -> str:
        if token in Token.Comment.PreprocFile:
            return "constant"
        if token in Token.Comment.Preproc:
            return "preproc"
        if token in Token.Comment:
            return "comment"
        if token in Token.Literal.String.Escape or token in Token.Literal.String.Interpol:
            return "special"
        if token in Token.Literal.String or token in Token.Literal.Number:
            return "constant"
        if token in Token.Name.Decorator:
            return "identifier" if language == "python" else "preproc"
        if token in Token.Keyword.Constant:
            return "statement" if language == "python" and value in self.statements else "constant"
        if token in Token.Name.Class:
            return "type"
        if token in Token.Name.Exception:
            return "statement" if language == "java" else "type"
        if token in Token.Name.Function or token in Token.Name.Builtin:
            return "identifier"
        if token in Token.Keyword or token in Token.Operator.Word:
            imports = {
                "python": {"from", "import", "lazy"},
                "java": {"import", "package", "native"},
                "cpp": {"import", "export", "module"},
            }
            if value in imports.get(language, set()) and value in self.preproc:
                return "preproc"
            if language == "python":
                return "statement" if value in self.statements else "normal"
            if language == "cpp" and value in {
                "new", "delete", "this", "friend", "using", "public", "private", "protected",
            }:
                return "statement"
            if language == "csharp" and value in {"this", "base", "using", "where"}:
                return "statement"
            if value in self.classes:
                return "type"
            return "statement" if value in self.statements else "normal"
        if token in Token.Name and value in self.classes:
            return "type"
        if token in Token.Operator and value in self.statements:
            return "statement"
        if token in Token.Punctuation and value in {"[[", "]]", "[:", ":]"}:
            return "special"
        return "normal"

    def highlight(self, path: Path | None = None) -> list[list[str]]:
        """Return categories per source character, preserving original offsets."""
        language = self.language(path)
        styles = ["normal"] * len(self.source)
        if language is not None:
            if get_lexer_by_name is None:
                raise ValueError("Install syntax support: python -m pip install Pygments")
            lexer = get_lexer_by_name(language)
            def paint(start, end, category):
                styles[start:end] = [category] * (end - start)

            string_mask = [False] * len(self.source)
            raw_mask = [False] * len(self.source)
            for start, token, value in lexer.get_tokens_unprocessed(self.source):
                paint(start, start + len(value), self._category(token, value, language))
                if token in Token.Literal.String:
                    string_mask[start:start + len(value)] = [True] * len(value)
                    # Pygments keeps C# verbatim strings in a single token.
                    if language == "csharp" and value.startswith(('@"', '$@"', '@$"')):
                        raw_mask[start:start + len(value)] = [True] * len(value)
                        for match in re.finditer('""', value):
                            paint(start + match.start(), start + match.end(), "special")
                if token in Token.Name.Decorator and language == "python" and value.startswith("@"):
                    paint(start, start + 1, "preproc")
                if token in Token.Comment and token not in Token.Comment.Preproc:
                    for match in re.finditer(r"\b(?:TODO|FIXME|XXX|NOTE|HACK|TBD)\b", value):
                        paint(start + match.start(), start + match.end(), "todo")
                    if language == "java":
                        for match in re.finditer(r"\{?@[A-Za-z]+", value):
                            if match[0] in self.specchars:
                                paint(start + match.start(), start + match.end(), "special")

            if language == "python":
                # Distinguish the soft-keyword type alias from the type() builtin.
                for match in re.finditer(r"(?m)^[ \t]*(type)[ \t]+([A-Za-z_]\w*)(?:\[[^\n]*?\])?[ \t]*=", self.source):
                    start = match.start(1)
                    if not string_mask[start] and styles[start] not in {"comment", "todo"}:
                        paint(start, match.end(1), "statement")
                        paint(match.start(2), match.end(2), "type")

            if language in {"c", "cpp", "csharp"}:
                # Some lexer versions do not know newer directives (#nullable,
                # #embed, #elifdef). Use the existing vocabulary for these too.
                for match in re.finditer(r"(?m)^[ \t]*(?:#|%:)[ \t]*(\w+)[^\n]*", self.source):
                    marker = match.start() + len(match[0]) - len(match[0].lstrip())
                    if match[1] not in self.preproc or string_mask[marker] or styles[marker] in {"comment", "todo"}:
                        continue
                    for index in range(marker, match.end()):
                        if styles[index] == "normal":
                            styles[index] = "preproc"

            # Preserve raw strings even where a lexer splits their delimiters.
            if language in {"cpp", "csharp"}:
                pattern = (r'(?<![\w\\])(?:u8|[uUL])?R"(?P<delimiter>[^ ()\\\t\r\n]{0,16})'
                           r'\((?P<body>.*?)\)(?P=delimiter)"' if language == "cpp" else
                           r'(?<!")(?P<dollars>\$*)(?P<quotes>"{3,})(?!")(?P<body>.*?)(?P=quotes)(?!")')
                for match in re.finditer(pattern, self.source, re.DOTALL):
                    if styles[match.start()] in {"comment", "todo"}:
                        continue
                    paint(match.start(), match.end(), "constant")
                    string_mask[match.start():match.end()] = [True] * len(match[0])
                    raw_mask[match.start():match.end()] = [True] * len(match[0])
                    if language == "cpp":
                        paint(match.start(), match.start("body"), "special")
                        paint(match.end("body"), match.end(), "special")

            # Java string escapes may be split into multiple lexer tokens.
            # Scan only string spans, never comments, code, verbatim/raw strings.
            if language in {"java", "csharp"}:
                for match in self.ESCAPE.finditer(self.source):
                    if all(string_mask[match.start():match.end()]) and not any(raw_mask[match.start():match.end()]):
                        paint(match.start(), match.end(), "special")
            if language in {"c", "cpp"}:
                for match in self.FORMAT.finditer(self.source):
                    if all(string_mask[match.start():match.end()]) and not any(raw_mask[match.start():match.end()]):
                        paint(match.start(), match.end(), "special")
            if language == "csharp":
                # The C# lexer emits a whole interpolated string as String.
                # Handle its common expression/alignment/format fields locally.
                for literal in re.finditer(r'(?:\$@|@\$|\$)"(?:""|\\.|[^"\\])*"', self.source):
                    if styles[literal.start()] in {"comment", "todo"}:
                        continue
                    for match in re.finditer(r'(?<!\{)\{(?!\{)([^{}]*)\}(?!\})', literal[0]):
                        start = literal.start() + match.start()
                        end = literal.start() + match.end()
                        paint(start, start + 1, "special")
                        paint(end - 1, end, "special")
                        expression, separator, _ = match[1].partition(":")
                        for offset, token, value in lexer.get_tokens_unprocessed(expression):
                            paint(start + 1 + offset, start + 1 + offset + len(value),
                                  "special" if value == "," else self._category(token, value, language))
                        if separator:
                            paint(start + 1 + len(expression), end - 1, "special")

        rows = []
        offset = 0
        for line in self.source.split("\n"):
            rows.append(styles[offset:offset + len(line)])
            offset += len(line) + 1
        return rows


class Document:
    def __init__(self, text: str = "") -> None:
        validate_text(text)

        self.lines = [MyString(line) for line in text.split("\n")]
        self.path: Path | None = None
        self.newline = "\n"
        self.modified = False
        self.buffer = MyString("")

    @classmethod
    def open_file(cls, path: Path) -> "Document":
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
    def __init__(self, document: Document, view: "TerminalView | None" = None) -> None:
        self.syntax_enabled = False
        self.history: list[Command] = []
        self.view = view
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
                        " | :o: open | :sy: syntax | / ?: search")
        self.running = True
        self.preferred_column = 0

    def clamp(self) -> None:
        self.row = max(0, min(self.row, len(self.document.lines) - 1))
        length = len(self.document.lines[self.row])
        limit = length if self.mode == "INSERT" else max(0, length - 1)
        self.column = max(0, min(self.column, limit))


    def execute(self, command: Command) -> None:
        """Execute a fresh command instance and remember it for LIFO undo."""
        command.execute(self)
        self.history.append(command)

    def undo(self) -> None:
        """Undo the last action, including its file write if there was one."""
        if self.history:
            self.history[-1].undo(self)
            self.history.pop()

    def execute_command(self) -> None:
        parts = self.command.strip().split(maxsplit=1)
        name = parts[0] if parts else ""
        argument = parts[1] if len(parts) == 2 else None
        action = None
        if name in {"w", "wq"}:
            action = {"w": WriteCommand, "wq": WriteQuitCommand}[name](
                Path(argument) if argument else None)
        elif name in {"o", "o!"} and argument:
            action = {"o": OpenFileCommand, "o!": ForceOpenFileCommand}[name](argument)
        elif argument is None and name in {"h", "help", "x", "q", "q!"}:
            action = {"h": HelpCommand, "help": HelpCommand, "x": ExitCommand,
                      "q": QuitCommand, "q!": ForceQuitCommand}[name]()
        elif name.isdigit():
            action = GoToLineCommand(int(name))
        elif name == "set" and argument == "num":
            action = ShowNumCommand()
        elif name == "sy" and argument is None:
            action = SyntaxCommand()
        elif name:
            self.message = "Unknown command: " + self.command
        try:
            if action is not None:
                self.execute(action)
        except (OSError, ValueError) as error:
            self.message = str(error)
        self.command = ""
        self.mode = "NORMAL"
        self.clamp()

    def show_help(self) -> None:
        self.execute(HelpCommand())

    def open_file(self, name: str) -> None:
        self.execute(ForceOpenFileCommand(name))

    def find_match(self, pattern: str, origin: tuple[int, int]) -> tuple[int, int] | None:
        search = ForwardSearchCommand if self.search_direction == "/" else BackwardSearchCommand
        return search.find_match(self.document, pattern, origin)

    def run_search(self) -> None:
        SearchPreviewCommand().execute(self)

    def execute_search(self) -> None:
        self.execute(ConfirmSearchCommand())

    def repeat_search(self, reverse: bool = False) -> None:
        self.execute(RepeatSearchCommand(reverse))

    def handle_key(self, key: str) -> None:
        if key == "ESC":
            self.execute(EscapeCommand())
            return
        if self.mode in {"COMMAND", "SEARCH"}:
            if key == "ENTER":
                if self.mode == "COMMAND":
                    self.execute_command()
                else:
                    self.execute_search()
            elif key == "BACKSPACE":
                self.command = self.command[:-1]
                if self.mode == "SEARCH":
                    self.run_search()
            elif len(key) == 1 and key.isprintable():
                self.command += key
                if self.mode == "SEARCH":
                    self.run_search()
            return

        arrows = {"LEFT": MoveLeftCommand, "RIGHT": MoveRightCommand,
                  "UP": MoveUpCommand, "DOWN": MoveDownCommand}
        if key in arrows:
            self.execute(arrows[key]())
            return
        if self.mode == "INSERT":
            actions = {"ENTER": SplitLineCommand, "BACKSPACE": BackspaceCommand,
                       "DELETE": DeleteCharacterCommand, "TAB": TabCommand}
            if key in actions:
                self.execute(actions[key]())
            elif len(key) == 1 and " " <= key <= "~":
                self.execute(InsertTextCommand(key))
            elif len(key) == 1:
                self.message = "ASCII input only"
        else:
            pending, self.pending = self.pending, ""
            if pending == "r" and len(key) == 1 and " " <= key <= "~":
                self.execute(ReplaceCharacterCommand(key))
            elif self.sequence == "d" and key == "i":
                self.sequence = "di"
                return
            elif self.sequence == "di" and key == "w":
                self.sequence = ""
                self.execute(DeleteInnerWordCommand())
            else:
                count, self.count = self.count, ""
                if len(key) == 1 and key.isdigit() and (key != "0" or count):
                    self.sequence = ""
                    self.count = count + key
                else:
                    self.sequence = ""
                    self.dispatch_normal(key, pending, count)

    def dispatch_normal(self, key, pending="", count=""):
        pairs = {("g", "g"): FirstLineCommand,
                 ("d", "d"): DeleteLineCommand,
                 ("d", "w"): DeleteWordCommand,
                 ("y", "y"): CopyLineCommand,
                 ("y", "w"): CopyWordCommand}
        actions = {"0": LineStartCommand, "$": LineEndCommand,
                   "w": NextWordCommand, "b": PreviousWordCommand,
                   "PG_UP": PageUpCommand, "PG_DN": PageDownCommand,
                   "x": DeleteCharacterCommand, "i": InsertModeCommand,
                   "I": InsertAtStartCommand, "A": AppendCommand,
                   "S": SubstituteLineCommand, "p": PasteCommand,
                   "n": RepeatSearchCommand, "N": ReverseRepeatSearchCommand,
                   ":": CommandModeCommand, "/": ForwardSearchCommand,
                   "?": BackwardSearchCommand}
        if (pending, key) in pairs:
            self.execute(pairs[pending, key]())
        elif key == "G":
            self.execute(GoToLineCommand(int(count)) if count else LastLineCommand())
        elif key in {"g", "d", "y", "r"}:
            self.pending = self.sequence = key
        elif key in actions:
            self.execute(actions[key]())


def safe_screen_text(text: str) -> str:

    return "".join(ch if " " <= ch <= "~" else "?" for ch in text)


class CursesTerminal:
    """Translate curses input to the controller's existing key names."""

    def __init__(self, screen) -> None:
        self.screen = screen
        screen.keypad(True)

    def read_key(self) -> str:
        key = self.screen.get_wch()
        if isinstance(key, int):
            return {
                curses.KEY_LEFT: "LEFT", curses.KEY_RIGHT: "RIGHT",
                curses.KEY_UP: "UP", curses.KEY_DOWN: "DOWN",
                curses.KEY_PPAGE: "PG_UP", curses.KEY_NPAGE: "PG_DN",
                curses.KEY_DC: "DELETE", curses.KEY_BACKSPACE: "BACKSPACE",
                curses.KEY_ENTER: "ENTER", curses.KEY_RESIZE: "RESIZE",
            }.get(key, "UNKNOWN")
        return {"\x1b": "ESC", "\r": "ENTER", "\n": "ENTER",
                "\b": "BACKSPACE", "\x7f": "BACKSPACE", "\t": "TAB",
                "\x03": "ESC", "\x04": "PG_DN", "\x15": "PG_UP"}.get(key, key)


class TerminalView:
    def __init__(self, screen=None) -> None:
        self.screen = screen
        self.show_num = False
        self.top = self.left = 0
        self.styles = {name: 0 for name in (
            "normal", "statement", "type", "preproc", "special", "comment",
            "constant", "identifier", "todo",
        )}
        self.highlighter = HighlightingDecorator("")
        self._syntax_key = None
        self._syntax_rows = []
        if screen is not None:
            self._init_colors()

    def _init_colors(self) -> None:
        if curses is None or not curses.has_colors():
            return
        curses.start_color()
        background = curses.COLOR_BLACK
        try:
            curses.use_default_colors()
            background = -1
        except curses.error:
            pass
        bright = getattr(curses, "COLORS", 8) >= 16

        offset = 8 if bright else 0
        colors = {
            "statement": (curses.COLOR_YELLOW + offset, False),
            "type": (curses.COLOR_GREEN + offset, not bright),
            "preproc": (curses.COLOR_BLUE + offset, not bright),
            "special": (curses.COLOR_RED + offset, not bright),
            "comment": (curses.COLOR_CYAN, False),
            "constant": (curses.COLOR_MAGENTA, False),
            "identifier": (curses.COLOR_CYAN, False),
        }
        for pair, (name, (foreground, bold)) in enumerate(colors.items(), 1):
            if pair >= curses.COLOR_PAIRS:
                break
            curses.init_pair(pair, foreground, background)
            self.styles[name] = curses.color_pair(pair) | (curses.A_BOLD if bold else 0)
        if curses.COLOR_PAIRS > 8:
            curses.init_pair(8, curses.COLOR_BLACK, curses.COLOR_YELLOW)
            self.styles["todo"] = curses.color_pair(8)

    def _put(self, row: int, column: int, text: str, attribute: int = 0) -> None:
        height, width = self.screen.getmaxyx()
        if not 0 <= row < height or not 0 <= column < width:
            return
        text = safe_screen_text(text)[:width - column]
        if not text:
            return
        try:
            self.screen.addnstr(row, column, text, len(text), attribute)
        except curses.error:
            if row != height - 1 or column + len(text) != width:
                raise

    def _syntax(self, editor: Editor) -> list[list[str]]:
        source = editor.document.text()
        key = (source, editor.document.path)
        if self._syntax_key != key:
            self.highlighter.source = source
            self._syntax_rows = self.highlighter.highlight(editor.document.path)
            self._syntax_key = key
        return self._syntax_rows

    def _draw_text(self, editor: Editor, row: int, screen_row: int,
                   number_width: int, text_width: int, syntax) -> None:
        if row >= len(editor.document.lines):
            self._put(screen_row, number_width, "~")
            return
        raw = str(editor.document.lines[row])
        text = raw.expandtabs(4)
        attributes = []
        column = 0
        for index, char in enumerate(raw):
            size = 4 - column % 4 if char == "\t" else 1
            category = syntax[row][index] if syntax is not None else "normal"
            attributes.extend([self.styles[category]] * size)
            column += size
        if editor.search_match is not None and editor.search_match[0] == row:
            _, start, length = editor.search_match
            end = len(raw[:start + length].expandtabs(4))
            start = len(raw[:start].expandtabs(4))
            for index in range(start, min(end, len(attributes))):
                attributes[index] |= curses.A_REVERSE
        visible = text[self.left:self.left + text_width]
        attributes = attributes[self.left:self.left + text_width]
        if number_width:
            self._put(screen_row, 0, f"{row + 1} ".rjust(number_width)[-number_width:])
        start = 0
        while start < len(visible):
            end = start + 1
            while end < len(visible) and attributes[end] == attributes[start]:
                end += 1
            self._put(screen_row, number_width + start, visible[start:end], attributes[start])
            start = end

    def draw(self, editor: Editor) -> None:
        if self.screen is None:
            raise ValueError("TerminalView requires a curses window")
        height, columns = self.screen.getmaxyx()
        if height < 1 or columns < 1:
            return
        width = max(1, columns - 1)
        body_height = max(0, height - 2)
        number_width = (min(len(str(len(editor.document.lines))) + 1, width - 1)
                        if self.show_num else 0)
        text_width = width - number_width
        raw = str(editor.document.lines[editor.row])
        screen_column = len(raw[:editor.column].expandtabs(4))
        self.top = max(0, min(self.top, editor.row))
        self.top = max(self.top, editor.row - max(1, body_height) + 1)
        self.left = min(self.left, screen_column)
        self.left = max(self.left, screen_column - text_width + 1)
        max_left = max(0, len(raw.expandtabs(4)) - text_width + (editor.mode == "INSERT"))
        self.left = min(self.left, max_left)
        syntax = self._syntax(editor) if editor.syntax_enabled else None

        self.screen.erase()
        for screen_row in range(body_height):
            self._draw_text(editor, self.top + screen_row, screen_row,
                            number_width, text_width, syntax)
        path = str(editor.document.path) if editor.document.path else "[New file]"
        status = (editor.mode + (" | SY" if editor.syntax_enabled else "")
                  + f" | {path} {'[+]' if editor.document.modified else ''}"
                  + f" | {editor.row + 1}:{editor.column + 1} {editor.sequence}")
        if height >= 2:
            self._put(height - 2, 0, safe_screen_text(status)[:width].ljust(width), curses.A_REVERSE)
        if editor.mode == "COMMAND":
            message = ":" + editor.command
        elif editor.mode == "SEARCH":
            message = editor.search_direction + editor.command
        else:
            message = editor.message
        self._put(height - 1, 0, message[-width:])

        if editor.mode in {"COMMAND", "SEARCH"} or body_height == 0:
            cursor_row = height - 1
            cursor_column = min(len(message), width - 1) if editor.mode in {"COMMAND", "SEARCH"} else 0
        else:
            cursor_row = editor.row - self.top
            cursor_column = number_width + screen_column - self.left
        self.screen.move(cursor_row, cursor_column)
        self.screen.refresh()


def run_editor(screen, document: Document) -> None:
    """curses.wrapper owns terminal setup and restores it on every exit path."""
    try:
        curses.curs_set(1)
    except curses.error:
        pass
    if hasattr(curses, "set_escdelay"):
        curses.set_escdelay(25)
    terminal = CursesTerminal(screen)
    view = TerminalView(screen)
    editor = Editor(document, view)
    while editor.running:
        view.draw(editor)
        key = terminal.read_key()
        if key != "RESIZE":
            editor.handle_key(key)


def main() -> int:
    if len(sys.argv) > 2:
        print("Usage: python MyVim.py [file]", file=sys.stderr)
        return 1
    if curses is None:
        print("curses is unavailable. On Windows run: python -m pip install windows-curses",
              file=sys.stderr)
        return 1
    if not sys.stdin.isatty() or not sys.stdout.isatty():
        print("Run in an interactive terminal, not redirected I/O", file=sys.stderr)
        return 1
    try:
        document = Document.open_file(Path(sys.argv[1])) if len(sys.argv) == 2 else Document()
        curses.wrapper(run_editor, document)
    except (OSError, ValueError, curses.error) as error:
        print(f"MyVim: {error}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        return 0
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
