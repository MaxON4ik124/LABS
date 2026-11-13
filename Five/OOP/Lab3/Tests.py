from pathlib import Path
import re

import pytest

import MyVim as editor_module
from MyVim import Document, Editor, TerminalView, safe_screen_text
from my_string import MyString


class FakeCurses:
    class error(Exception):
        pass

    A_BOLD = 1 << 20
    A_REVERSE = 1 << 21
    COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW = 0, 1, 2, 3
    COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN = 4, 5, 6
    COLORS, COLOR_PAIRS = 16, 64
    KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN = 1000, 1001, 1002, 1003
    KEY_PPAGE, KEY_NPAGE, KEY_DC, KEY_BACKSPACE = 1004, 1005, 1006, 1007
    KEY_ENTER, KEY_RESIZE = 1008, 1009

    def __init__(self):
        self.pairs = {}

    def has_colors(self):
        return True

    def start_color(self):
        pass

    def use_default_colors(self):
        pass

    def init_pair(self, number, foreground, background):
        self.pairs[number] = (foreground, background)

    def color_pair(self, number):
        return number << 8

    def curs_set(self, visibility):
        return 1

    def set_escdelay(self, milliseconds):
        pass


class FakeWindow:
    """A cell/attribute grid, so rendering tests do not need a live terminal."""
    def __init__(self, height=24, width=80, keys=()):
        self.height, self.width = height, width
        self.keys = list(keys)
        self.cursor = (0, 0)
        self.refreshes = 0
        self.keypad_enabled = False
        self.erase()

    def getmaxyx(self):
        return self.height, self.width

    def erase(self):
        self.cells = [[" "] * self.width for _ in range(self.height)]
        self.attributes = [[0] * self.width for _ in range(self.height)]

    def addnstr(self, row, column, text, size, attribute=0):
        assert 0 <= row < self.height
        assert 0 <= column < self.width
        assert column + min(size, len(text)) <= self.width
        assert "\x1b" not in text
        for offset, char in enumerate(text[:size]):
            self.cells[row][column + offset] = char
            self.attributes[row][column + offset] = attribute

    def move(self, row, column):
        assert 0 <= row < self.height
        assert 0 <= column < self.width
        self.cursor = row, column

    def refresh(self):
        self.refreshes += 1

    def keypad(self, enabled):
        self.keypad_enabled = enabled

    def get_wch(self):
        assert self.keys, "The event loop requested unexpected extra input"
        return self.keys.pop(0)

    def row_text(self, row):
        return "".join(self.cells[row]).rstrip()


@pytest.fixture
def fake_curses(monkeypatch):
    module = FakeCurses()
    monkeypatch.setattr(editor_module, "curses", module)
    return module


def press(editor: Editor, *keys: str) -> Editor:
    """Feed a sequence of key names to the editor."""
    for key in keys:
        editor.handle_key(key)
    return editor


def type_command(editor: Editor, command: str, *suffix: str) -> Editor:
    """Open the ':' command line, type each character, then press the suffix keys."""
    editor.handle_key(":")
    press(editor, *command, *suffix)
    return editor


# ---------------------------------------------------------------------------
# Document
# ---------------------------------------------------------------------------


def test_insert_delete_and_storage():
    document = Document()
    editor = Editor(document)
    editor.mode = "INSERT"
    assert isinstance(document.lines[0], MyString)

    editor.execute(editor_module.InsertTextCommand("abc"))
    editor.column = 1
    editor.execute(editor_module.DeleteCharacterCommand())
    assert document.text() == "ac"
    assert document.modified is True

    editor.column = 2
    editor.execute(editor_module.DeleteCharacterCommand())
    assert document.text() == "ac"


def test_split_and_merge():
    document = Document("hello")
    editor = Editor(document)
    editor.mode = "INSERT"
    editor.column = 2
    editor.execute(editor_module.SplitLineCommand())
    assert document.text() == "he\nllo"
    editor.execute(editor_module.BackspaceCommand())
    assert (editor.row, editor.column) == (0, 2)
    assert document.text() == "hello"


def test_empty_lines():
    document = Document()
    editor = Editor(document)
    editor.mode = "INSERT"
    editor.execute(editor_module.SplitLineCommand())
    assert document.text() == "\n"

    editor.execute(editor_module.BackspaceCommand())
    editor.execute(editor_module.InsertTextCommand("x"))
    editor.execute(editor_module.DeleteLineCommand())
    assert document.text() == ""
    assert len(document.lines) == 1
    editor.execute(editor_module.BackspaceCommand())
    assert (editor.row, editor.column) == (0, 0)


@pytest.mark.parametrize(
    "data",
    [b"", b"abc", b"abc\n", b"a\n\nb\n", b"a\r\nb\r\n", b"\t hi  "],
    ids=["empty", "no-newline", "trailing-lf", "blank-line", "crlf", "tabs-spaces"],
)
def test_file_round_trips(tmp_path: Path, data: bytes):
    path = tmp_path / "test.txt"
    path.write_bytes(data)

    document = Document.open_file(path)
    document.save()

    assert path.read_bytes() == data
    assert document.modified is False


def test_new_file_and_failed_save(tmp_path: Path):
    path = tmp_path / "new.txt"
    document = Document.open_file(path)
    assert path.exists() is False

    Editor(document).execute(editor_module.InsertTextCommand("new"))
    with pytest.raises(OSError):
        document.save(tmp_path / "missing" / "file.txt")
    assert document.modified is True
    assert document.path == path.resolve()

    document.save()
    assert path.read_bytes() == b"new"


@pytest.mark.parametrize(
    "text",
    ["Привет", "\x1b[2J", "\x00", "a\rb"],
    ids=["cyrillic", "escape-sequence", "nul", "carriage-return"],
)
def test_reject_unsupported_text(text: str):
    with pytest.raises(ValueError):
        Document(text)


def test_reject_newline_through_insert():
    document = Document()
    with pytest.raises(ValueError):
        Editor(document).execute(editor_module.InsertTextCommand("\n"))


def test_mixed_newlines_rejected(tmp_path: Path):
    path = tmp_path / "mixed.txt"
    path.write_bytes(b"a\r\nb\n")
    with pytest.raises(ValueError):
        Document.open_file(path)


def test_copy_line_is_a_real_copy():
    document = Document("aaa\nbbb")
    Editor(document).execute(editor_module.CopyLineCommand())
    assert str(document.buffer) == "aaa\n"

    document.lines[0].append("X")
    assert str(document.buffer) == "aaa\n"


def test_copy_line_on_single_line():
    document = Document("only")
    Editor(document).execute(editor_module.CopyLineCommand())
    assert str(document.buffer) == "only\n"


def test_copy_word():
    document = Document("hello world")
    Editor(document).execute(editor_module.CopyWordCommand())
    assert str(document.buffer) == "hello"

    editor = Editor(document)
    editor.column = 6
    editor.execute(editor_module.CopyWordCommand())
    assert str(document.buffer) == "world"


def test_paste_line_buffer_inserts_new_line_below():
    document = Document("one\ntwo")
    Editor(document).execute(editor_module.CopyLineCommand())
    editor = Editor(document)
    editor.execute(editor_module.PasteCommand())
    assert (editor.row, editor.column) == (1, 0)
    assert document.text() == "one\none\ntwo"


def test_paste_word_buffer_inserts_after_cursor():
    document = Document("hello world")
    Editor(document).execute(editor_module.CopyWordCommand())
    editor = Editor(document)
    editor.execute(editor_module.PasteCommand())
    assert (editor.row, editor.column) == (0, 6)
    assert document.text() == "hhelloello world"


def test_paste_works_with_cursor_at_end_of_line():
    document = Document("ab")
    Editor(document).execute(editor_module.CopyWordCommand())
    editor = Editor(document)
    editor.mode = "INSERT"
    editor.column = 2
    editor.execute(editor_module.PasteCommand())
    assert document.text() == "abab"


# ---------------------------------------------------------------------------
# Editor (controller)
# ---------------------------------------------------------------------------


@pytest.fixture
def tall_terminal(monkeypatch: pytest.MonkeyPatch):
    """Pretend the terminal is 80x12 so page motion is predictable."""
    monkeypatch.setattr(
        editor_module.shutil, "get_terminal_size", lambda *a, **k: (80, 12)
    )


def test_page_scrolling(tall_terminal):
    lines = "\n".join(f"line{i}" for i in range(1, 51))
    editor = Editor(Document(lines))

    press(editor, "PG_DN")
    assert (editor.row, editor.column) == (10, 0)
    press(editor, "PG_DN")
    assert editor.row == 20
    press(editor, "PG_UP")
    assert editor.row == 10
    press(editor, "PG_UP", "PG_UP")
    assert editor.row == 0
    press(editor, "PG_DN", "PG_DN", "PG_DN", "PG_DN", "PG_DN")
    assert editor.row == 49


def test_insert_still_works_with_diw_command():

    editor = Editor(Document())
    press(editor, "i", "H", "i", "ESC")
    assert editor.document.text() == "Hi"
    assert editor.mode == "NORMAL"


    editor = Editor(Document("one\ntwo\nthree"))
    press(editor, "d", "d")
    assert editor.document.text() == "two\nthree"


    editor = Editor(Document("abc def"))
    press(editor, "d", "i", "w")
    assert editor.mode == "NORMAL"

    editor = Editor(Document())
    press(editor, "d", "i", "ESC", "i", "Z", "ESC")
    assert editor.document.text() == "Z"


    editor = Editor(Document("aaa bbb ccc"))
    press(editor, "d", "ESC", "w")
    assert editor.row == 0


def test_insert_normal_delete():
    editor = Editor(Document())
    press(editor, "i", "H", "i", "ESC")
    assert editor.document.text() == "Hi"
    assert editor.mode == "NORMAL"
    press(editor, "0", "x")
    assert editor.document.text() == "i"


def test_enter_backspace_and_append():
    editor = Editor(Document("abc"))
    press(editor, "$", "i", "ENTER", "x", "BACKSPACE", "BACKSPACE", "ESC")
    assert editor.document.text() == "abc"
    assert (editor.row, editor.column) == (0, 2)


def test_dd_and_cancel_pending_command():
    editor = Editor(Document("one\ntwo"))
    press(editor, "d", "ESC", "d")
    assert editor.document.text() == "one\ntwo"

    press(editor, "d")
    assert editor.document.text() == "two"

    press(editor, "d", "d", "d", "d")
    assert editor.document.text() == ""


@pytest.mark.parametrize(
    ("count_keys", "expected"),
    [
        (("3", "G"), (2, 0)),
        (("1", "G"), (0, 0)),
        (("5", "G"), (4, 0)),
    ],
    ids=["third", "first", "fifth"],
)
def test_ng_moves_to_nth_line(count_keys, expected):
    editor = Editor(Document("one\ntwo\nthree\nfour\nfive"))
    press(editor, *count_keys)
    assert (editor.row, editor.column) == expected


def test_ng_beyond_last_line_goes_to_end():
    editor = Editor(Document("one\ntwo\nthree"))
    press(editor, "9", "9", "G")
    assert (editor.row, editor.column) == (2, 0)


def test_plain_g_goes_to_last_line():
    editor = Editor(Document("one\ntwo\nthree"))
    press(editor, "G")
    assert (editor.row, editor.column) == (2, 0)


def test_zero_alone_is_motion_not_count():
    editor = Editor(Document("abcdef"))
    press(editor, "$")
    assert editor.column == 5
    press(editor, "0")
    assert editor.column == 0
    press(editor, "1", "0", "G")
    assert editor.row == 0


def test_count_discarded_before_other_command():
    editor = Editor(Document("one\ntwo\nthree"))
    press(editor, "2", "0", "g", "g")
    assert (editor.row, editor.column) == (0, 0)


def test_enter_in_insert_mode_splits_line():
    editor = Editor(Document("hello"))
    press(editor, "i", "RIGHT", "RIGHT", "ENTER")
    assert editor.document.text() == "he\nllo"
    assert (editor.row, editor.column) == (1, 0)
    assert editor.preferred_column == 0

    press(editor, "ESC")
    assert (editor.row, editor.column) == (1, 0)


def test_yy_p_pastes_line_below():
    editor = Editor(Document("one\ntwo"))
    press(editor, "y", "y", "p")
    assert editor.document.text() == "one\none\ntwo"
    assert (editor.row, editor.column) == (1, 0)


def test_yw_p_pastes_word():
    editor = Editor(Document("hello world"))
    press(editor, "y", "w", "p")
    assert editor.document.text() == "hhelloello world"
    assert (editor.row, editor.column) == (0, 6)


def test_w_still_moves_when_not_yanked():
    editor = Editor(Document("hello world foo"))
    press(editor, "w")
    assert editor.column == 4
    assert str(editor.document.buffer) == ""


def test_preferred_column_and_boundaries():
    editor = Editor(Document("abcdef\nx\nabcdef"))
    press(editor, "$")
    assert editor.column == 5
    press(editor, "DOWN")
    assert editor.column == 0
    press(editor, "DOWN")
    assert editor.column == 5
    press(editor, "DOWN", "RIGHT")
    assert (editor.row, editor.column) == (2, 5)


@pytest.mark.parametrize("key", ["h", "j", "k", "l"])
def test_hjkl_do_not_move_in_normal_mode(key: str):
    editor = Editor(Document("abc\ndef\nghi"))
    press(editor, "DOWN", "RIGHT")

    press(editor, key)
    assert (editor.row, editor.column) == (1, 1)
    assert editor.document.text() == "abc\ndef\nghi"


@pytest.mark.parametrize("mode", ["NORMAL", "INSERT"])
def test_arrows_move_in_normal_and_insert_modes(mode: str):
    editor = Editor(Document("abc\ndef"))
    editor.mode = mode
    press(editor, "RIGHT", "DOWN")
    assert (editor.row, editor.column) == (1, 1)
    press(editor, "LEFT", "UP")
    assert (editor.row, editor.column) == (0, 0)


def test_save_quit_and_command_cancel(tmp_path: Path):
    editor = Editor(Document())


    press(editor, "i", "x", "ESC", ":", "q", "ENTER")
    assert editor.running is True
    assert "Unsaved" in editor.message

    path = tmp_path / "file with spaces.txt"
    type_command(editor, f"w {path}", "ENTER")
    assert path.read_bytes() == b"x"


    press(editor, ":", "q", "ESC")
    assert editor.running is True

    press(editor, ":", "q", "ENTER")
    assert editor.running is False


def test_write_error_and_force_quit():
    editor = Editor(Document("abc"))
    press(editor, ":", "w", "ENTER")
    assert "No file name" in editor.message

    press(editor, "x", ":", "q", "!", "ENTER")
    assert editor.running is False


def test_write_and_quit(tmp_path: Path):
    path = tmp_path / "saved.txt"
    editor = Editor(Document("abc"))
    type_command(editor, f"wq {path}", "ENTER")
    assert editor.running is False
    assert path.read_bytes() == b"abc"


def test_tab_and_non_ascii_input():
    editor = Editor(Document())
    press(editor, "i", "TAB", "я")
    assert editor.document.text() == "    "
    assert editor.column == 4
    assert "ASCII" in editor.message


# ---------------------------------------------------------------------------
# TerminalView
# ---------------------------------------------------------------------------


def test_scrolling_and_safe_output(fake_curses):
    screen = FakeWindow(4, 10)
    view = TerminalView(screen)
    editor = Editor(Document("\t0123456789\nsecond\nthird"), view)
    editor.column = 10
    view.draw(editor)
    assert view.left > 0
    assert "NORMAL" in screen.row_text(2)
    assert screen.refreshes == 1

    editor.row, editor.column = 2, 0
    view.draw(editor)
    assert view.top == 1
    assert screen.row_text(0) == "second"
    assert screen.row_text(1) == "third"
    assert safe_screen_text("a\x1b\nb") == "a??b"


# ---------------------------------------------------------------------------
# Commands: execution, routing and complete state restoration
# ---------------------------------------------------------------------------


def editor_state(editor: Editor):
    """Compare values, not mutable MyString objects or the history stack."""
    return (
        {key: value for key, value in vars(editor).items()
         if key not in {"document", "history"}},
        editor.document,
        editor.document.text(),
        editor.document.path,
        editor.document.newline,
        editor.document.modified,
        str(editor.document.buffer),
        editor.view.show_num if editor.view is not None else None,
    )


COMMAND_CLASSES = [
    cls for cls in vars(editor_module).values()
    if isinstance(cls, type) and cls is not editor_module.Command
    and issubclass(cls, editor_module.Command)
]


@pytest.mark.parametrize("command_class", COMMAND_CLASSES, ids=lambda cls: cls.__name__)
def test_every_command_can_restore_editor_and_document(command_class, tmp_path):
    editor = Editor(Document("alpha beta\nsecond alpha\nthird"), TerminalView())
    editor.document.path = tmp_path / "original.txt"
    editor.document.newline = "\r\n"
    editor.document.modified = True
    editor.document.buffer = MyString("clipboard\n")
    editor.row, editor.column, editor.preferred_column = 0, 2, 2
    editor.command = "alpha"
    editor.search_origin = (0, 0)
    editor.last_search = "alpha"
    editor.last_direction = "/"
    target = tmp_path / "target.txt"
    target.write_bytes(b"previous\r\ncontents")
    arguments = {
        "GoToLineCommand": (2,),
        "ReplaceCharacterCommand": ("Z",),
        "InsertTextCommand": ("new",),
        "OpenFileCommand": (str(target),),
        "ForceOpenFileCommand": (str(target),),
        "WriteCommand": (target,),
        "WriteQuitCommand": (target,),
    }
    action = command_class(*arguments.get(command_class.__name__, ()))
    before = editor_state(editor)
    action.execute(editor)
    action.undo(editor)
    assert editor_state(editor) == before
    assert target.read_bytes() == b"previous\r\ncontents"
    assert not editor.document.path.exists()


@pytest.mark.parametrize("keys, command_name", [
    (("LEFT",), "MoveLeftCommand"),
    (("RIGHT",), "MoveRightCommand"),
    (("UP",), "MoveUpCommand"),
    (("DOWN",), "MoveDownCommand"),
    (("PG_UP",), "PageUpCommand"),
    (("PG_DN",), "PageDownCommand"),
    (("0",), "LineStartCommand"),
    (("$",), "LineEndCommand"),
    (("w",), "NextWordCommand"),
    (("b",), "PreviousWordCommand"),
    (("g", "g"), "FirstLineCommand"),
    (("G",), "LastLineCommand"),
    (("2", "G"), "GoToLineCommand"),
    (("i",), "InsertModeCommand"),
    (("I",), "InsertAtStartCommand"),
    (("A",), "AppendCommand"),
    (("S",), "SubstituteLineCommand"),
    (("x",), "DeleteCharacterCommand"),
    (("r", "Z"), "ReplaceCharacterCommand"),
    (("d", "w"), "DeleteWordCommand"),
    (("d", "d"), "DeleteLineCommand"),
    (("d", "i", "w"), "DeleteInnerWordCommand"),
    (("y", "y"), "CopyLineCommand"),
    (("y", "w"), "CopyWordCommand"),
    (("p",), "PasteCommand"),
    (("/",), "ForwardSearchCommand"),
    (("?",), "BackwardSearchCommand"),
    (("n",), "RepeatSearchCommand"),
    (("N",), "ReverseRepeatSearchCommand"),
    ((":",), "CommandModeCommand"),
    (("ESC",), "EscapeCommand"),
    (("i", "ENTER"), "SplitLineCommand"),
    (("i", "BACKSPACE"), "BackspaceCommand"),
    (("i", "DELETE"), "DeleteCharacterCommand"),
    (("i", "TAB"), "TabCommand"),
    (("i", "a"), "InsertTextCommand"),
])
def test_keyboard_dispatches_command_classes(keys, command_name):
    editor = Editor(Document("alpha beta\nsecond"))
    press(editor, *keys)
    assert type(editor.history.commands(editor)[-1]).__name__ == command_name


@pytest.mark.parametrize("name, command_name", [
    ("h", "HelpCommand"), ("help", "HelpCommand"),
    ("q", "QuitCommand"), ("q!", "ForceQuitCommand"),
    ("x", "ExitCommand"), ("2", "GoToLineCommand"),
    ("w", "WriteCommand"), ("wq", "WriteQuitCommand"),
    ("o", "OpenFileCommand"), ("o!", "ForceOpenFileCommand"),
])
def test_ex_commands_dispatch_classes(name, command_name, tmp_path):
    editor = Editor(Document("one\ntwo\nthree"))
    path = tmp_path / "file.txt"
    path.write_text("existing", encoding="ascii")
    command = f"{name} {path}" if name in {"w", "wq", "o", "o!"} else name
    type_command(editor, command, "ENTER")
    assert type(editor.history.commands(editor)[-1]).__name__ == command_name


def test_delete_next_word_and_undo():
    editor = Editor(Document("alpha beta gamma"))
    press(editor, "d", "w")
    assert editor.document.text() == "beta gamma"
    editor.undo()
    assert editor.document.text() == "alpha beta gamma"
    assert editor.document.modified is False


@pytest.mark.parametrize("column, expected", [
    (0, " beta gamma"), (2, " beta gamma"),
    (6, "alpha  gamma"), (13, "alpha beta "),
])
def test_delete_inner_word_and_undo(column, expected):
    editor = Editor(Document("alpha beta gamma"))
    editor.column = column
    press(editor, "d", "i", "w")
    assert editor.document.text() == expected
    editor.undo()
    assert editor.document.text() == "alpha beta gamma"
    assert editor.column == column


def test_substitute_clears_line_without_removing_it():
    editor = Editor(Document("one\ntwo\nthree"))
    editor.row, editor.column = 1, 1
    press(editor, "S")
    assert editor.document.text() == "one\n\nthree"
    assert editor.mode == "INSERT"
    assert (editor.row, editor.column) == (1, 0)
    editor.undo()
    assert editor.document.text() == "one\ntwo\nthree"
    assert editor.mode == "NORMAL"
    assert (editor.row, editor.column) == (1, 1)


def test_ex_line_number_is_one_based():
    editor = Editor(Document("one\ntwo\nthree"))
    type_command(editor, "2", "ENTER")
    assert (editor.row, editor.column) == (1, 0)


@pytest.mark.parametrize("command_class", [
    editor_module.WriteCommand, editor_module.WriteQuitCommand,
])
@pytest.mark.parametrize("existing", [False, True])
def test_undo_write_restores_file_and_document(command_class, existing, tmp_path):
    target = tmp_path / "saved.txt"
    if existing:
        target.write_bytes(b"old\r\nfile")
    editor = Editor(Document("new\ntext"))
    editor.document.newline = "\r\n"
    editor.document.modified = True
    before = editor_state(editor)
    editor.execute(command_class(target))
    assert target.read_bytes() == b"new\r\ntext"
    assert editor.document.path == target.resolve()
    assert not editor.document.modified
    assert editor.running is (command_class is editor_module.WriteCommand)
    editor.undo()
    assert editor_state(editor) == before
    assert target.exists() is existing
    if existing:
        assert target.read_bytes() == b"old\r\nfile"


def test_failed_write_is_not_added_to_history(tmp_path):
    editor = Editor(Document("data"))
    editor.document.modified = True
    path = tmp_path / "missing" / "file.txt"
    type_command(editor, f"w {path}", "ENTER")
    assert not any(isinstance(action, editor_module.WriteCommand)
                   for action in editor.history.commands(editor))
    assert editor.document.modified
    assert editor.document.path is None
    assert not path.exists()


def test_open_and_help_undo_restore_original_document(tmp_path):
    path = tmp_path / "other.txt"
    path.write_text("other", encoding="ascii")
    original = Document("original")
    editor = Editor(original)
    editor.execute(editor_module.OpenFileCommand(str(path)))
    assert editor.document.text() == "other"
    editor.execute(editor_module.HelpCommand())
    assert editor.document.path == editor_module.HELP_PATH.resolve()
    editor.undo()
    assert editor.document.text() == "other"
    editor.undo()
    assert editor.document is original
    assert editor.document.text() == "original"
    editor.undo() 


def test_search_and_reverse_repeat_are_commands():
    editor = Editor(Document("one two one two"))
    press(editor, "/", *"two", "ENTER")
    assert (editor.row, editor.column) == (0, 4)
    assert isinstance(editor.history.commands(editor)[-1], editor_module.ConfirmSearchCommand)
    press(editor, "n")
    assert editor.column == 12
    press(editor, "N")
    assert editor.column == 4
    editor.undo()
    assert editor.column == 12
    assert editor.last_search == "two"



def test_document_only_exposes_storage_and_file_operations():
    methods = {name for name, value in vars(Document).items()
               if isinstance(value, classmethod) or callable(value)}
    assert methods == {"__init__", "open_file", "text", "save"}


@pytest.mark.parametrize("command_class", [
    editor_module.DeleteCharacterCommand,
    editor_module.DeleteLineCommand,
    editor_module.BackspaceCommand,
])
def test_empty_document_edit_does_not_mark_it_modified(command_class):
    editor = Editor(Document())
    editor.execute(command_class())
    assert editor.document.text() == ""
    assert len(editor.document.lines) == 1
    assert not editor.document.modified
    editor.undo()
    assert editor.document.text() == ""


def test_backspace_removes_character_and_undo_restores_cursor():
    editor = Editor(Document("abc"))
    editor.mode = "INSERT"
    editor.column = 2
    before = editor_state(editor)
    editor.execute(editor_module.BackspaceCommand())
    assert editor.document.text() == "ac"
    assert editor.column == 1
    editor.undo()
    assert editor_state(editor) == before


@pytest.mark.parametrize("command_class", [
    editor_module.InsertTextCommand, editor_module.ReplaceCharacterCommand,
])
@pytest.mark.parametrize("text", ["\n", "\u041f\u0440\u0438\u0432\u0435\u0442", "\x00"])
def test_invalid_insert_or_replace_is_atomic(command_class, text):
    editor = Editor(Document("abc"))
    before = editor_state(editor)
    with pytest.raises(ValueError):
        editor.execute(command_class(text))
    assert editor_state(editor) == before
    assert not editor.history.commands(editor)


@pytest.mark.parametrize("command_class, pattern, origin, expected", [
    (editor_module.ForwardSearchCommand, "alpha", (0, 0), (1, 5)),
    (editor_module.ForwardSearchCommand, "alpha", (1, 5), (2, 0)),
    (editor_module.ForwardSearchCommand, "missing", (0, 0), None),
    (editor_module.BackwardSearchCommand, "alpha", (2, 0), (1, 5)),
    (editor_module.BackwardSearchCommand, "alpha", (1, 5), (0, 0)),
    (editor_module.BackwardSearchCommand, "missing", (2, 0), None),
])
def test_search_algorithms_belong_to_commands(command_class, pattern, origin, expected):
    document = Document("alpha beta\nbeta alpha\nalpha")
    assert command_class.find_match(document, pattern, origin) == expected


# ---------------------------------------------------------------------------
# Number gutter, text scrolling and cursor coordinates
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("mode", ["NORMAL", "INSERT"])
@pytest.mark.parametrize("text", ["abcdef", "0123456789abcdef", "\tabcdefghijkl"])
def test_numbered_view_cursor_reaches_every_character(mode, text, fake_curses):
    screen = FakeWindow(4, 12)
    view = TerminalView(screen)
    view.show_num = True
    editor = Editor(Document(text), view)
    editor.mode = mode
    expanded = text.expandtabs(4)
    limit = len(text) if mode == "INSERT" else len(text) - 1
    for column in range(limit + 1):
        editor.column = column
        view.draw(editor)
        body_line = screen.row_text(0)
        cursor_row, cursor_column = screen.cursor
        screen_column = len(text[:column].expandtabs(4))
        assert body_line == "1 " + expanded[view.left:view.left + 9]
        assert (cursor_row, cursor_column) == (0, 2 + screen_column - view.left)
        assert 2 <= cursor_column <= 10
        if column < len(text):
            assert body_line[cursor_column] == expanded[screen_column]
        else:
            assert cursor_column == len(body_line)


def test_number_gutter_uses_document_rows_after_scrolling(fake_curses):
    screen = FakeWindow(4, 12)
    view = TerminalView(screen)
    view.show_num = True
    editor = Editor(Document("\n".join(["abcdefghijklmnop"] * 12)), view)
    editor.row, editor.column = 11, 15
    view.draw(editor)
    assert view.top == 10
    assert view.left == 8
    assert screen.row_text(0) == "11 ijklmnop"
    assert screen.row_text(1) == "12 ijklmnop"
    assert screen.cursor == (1, 10)


def test_number_gutter_does_not_shift_search_highlight(fake_curses):
    screen = FakeWindow(4, 12)
    view = TerminalView(screen)
    view.show_num = True
    editor = Editor(Document("abcdefghijklmnop"), view)
    editor.column = 15
    editor.search_match = (0, 13, 3)
    view.draw(editor)
    assert screen.row_text(0) == "1 hijklmnop"
    assert screen.attributes[0][8:11] == [fake_curses.A_REVERSE] * 3
    assert screen.attributes[0][:8] == [0] * 8
    assert screen.cursor == (0, 10)


@pytest.mark.parametrize("mode, command", [("COMMAND", "set num"), ("SEARCH", "abc")])
def test_number_gutter_does_not_shift_prompt_cursor(mode, command, fake_curses):
    screen = FakeWindow(4, 20)
    view = TerminalView(screen)
    view.show_num = True
    editor = Editor(Document("abc"), view)
    editor.mode, editor.command = mode, command
    view.draw(editor)
    assert screen.cursor == (3, len(command) + 1)


@pytest.mark.parametrize("terminal_width", [1, 2, 3])
def test_number_gutter_preserves_text_space_in_narrow_terminal(terminal_width, fake_curses):
    screen = FakeWindow(4, terminal_width)
    view = TerminalView(screen)
    view.show_num = True
    editor = Editor(Document("abcdef"), view)
    editor.column = 5
    view.draw(editor)
    width = max(1, terminal_width - 1)
    assert screen.cursor == (0, width - 1)
    assert "f" in screen.row_text(0)


# ---------------------------------------------------------------------------
# Vim-style syntax vocabulary (source parsing/rendering is separate)
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("field", ["statements", "classes", "preproc", "specchars"])
def test_highlighting_categories_are_nonempty_string_sets(field):
    decorator = editor_module.HighlightingDecorator("source text")
    words = getattr(decorator, field)
    assert isinstance(words, set)
    assert words
    assert all(isinstance(word, str) and word for word in words)
    assert decorator.source == "source text"
    assert decorator.comment_starts == {"#", "//"}


@pytest.mark.parametrize("field, words", [
    # Python: soft keywords and import directives have separate Vim groups.
    ("statements", "async await def class if elif else for while break continue match case type try except finally raise yield lambda"),
    ("classes", "BaseException ExceptionGroup ValueError TypeError SyntaxError Warning"),
    ("preproc", "from import"),
    # Java: declarations/modifiers are Type; module and control flow are Statement.
    ("statements", "assert if switch case default for do while break continue new instanceof try catch finally throw yield when requires exports opens uses provides"),
    ("classes", "boolean byte char short int long float double void class @interface interface enum record extends implements permits sealed non-sealed default public private protected static final synchronized throws"),
    ("preproc", "import package native static to with"),
    # C#: query/accessor words and types/modifiers.
    ("statements", "if switch foreach using lock nameof sizeof typeof stackalloc get set init add remove where select from join orderby yield await"),
    ("classes", "bool byte decimal dynamic nint nuint object string uint ulong ushort var record delegate abstract internal partial readonly sealed unsafe unmanaged"),
    ("preproc", "#define #undef #if #elif #else #endif #region #endregion #pragma #nullable #line #error #warning #load #r"),
    # C: include C11/C23 and common standard typedefs.
    ("statements", "if else switch case for do while goto sizeof _Alignof _Generic _Static_assert alignof static_assert typeof typeof_unqual"),
    ("classes", "int char float double void struct union enum typedef const volatile restrict _Atomic _Bool _Complex _BitInt _Decimal32 _Decimal64 _Decimal128 _Alignas _Thread_local size_t ptrdiff_t int32_t uint64_t FILE"),
    ("preproc", "#include #define #undef #if #ifdef #ifndef #elif #elifdef #elifndef #else #endif #embed #pragma"),
    # C++: casts, alternative operators, concepts and coroutines.
    ("statements", "new delete this friend public protected private co_await co_return co_yield requires noexcept const_cast static_cast dynamic_cast reinterpret_cast and_eq bitand bitor compl not_eq or_eq xor_eq"),
    ("classes", "class typename template namespace bool wchar_t char8_t char16_t char32_t constexpr consteval constinit decltype thread_local concept mutable explicit virtual"),
    ("preproc", "import module export"),
])
def test_highlighting_contains_language_keywords(field, words):
    decorator = editor_module.HighlightingDecorator("")
    assert set(words.split()) <= getattr(decorator, field)


def test_highlighting_covers_python_reserved_words():
    import keyword
    decorator = editor_module.HighlightingDecorator("")
    vocabulary = decorator.statements | decorator.classes | decorator.preproc
    assert set(keyword.kwlist) <= vocabulary


@pytest.mark.parametrize("token", [
    r"\a", r"\b", r"\f", r"\n", r"\r", r"\t", r"\v", r"\0",
    r"\\", r"\'", r'\"', r"\?", r"\s", r"\e",
    r"\x", r"\u", r"\U", r"\N{",
    "%d", "%s", "%f", "%lld", "%zu", "%lc", "%ls", "%lf", "%%",
    "!r", "!s", "!a", "{", "}", "[[", "]]",
    "@param", "@return", "@throws", "{@code", "{@link",
])
def test_highlighting_contains_special_tokens_and_prefixes(token):
    decorator = editor_module.HighlightingDecorator("")
    assert token in decorator.specchars


def test_highlighting_escapes_are_literal_text_not_control_characters():
    decorator = editor_module.HighlightingDecorator("")
    assert r"\n" in decorator.specchars
    assert "\n" not in decorator.specchars
    assert r"\t" in decorator.specchars
    assert "\t" not in decorator.specchars


def test_highlighting_leaves_ordinary_constants_and_names_out():
    decorator = editor_module.HighlightingDecorator("")
    vocabulary = decorator.statements | decorator.classes | decorator.preproc | decorator.specchars
    assert {"true", "false", "null", "nullptr", "123", "0", "user_variable"}.isdisjoint(vocabulary)


def test_highlighting_sets_belong_to_each_instance():
    first = editor_module.HighlightingDecorator("")
    second = editor_module.HighlightingDecorator("")
    for field in ("statements", "classes", "preproc", "specchars"):
        getattr(first, field).add("custom_token")
        assert "custom_token" not in getattr(second, field)


# ---------------------------------------------------------------------------
# curses input, syntax rendering and event loop
# ---------------------------------------------------------------------------


def source_category(source, filename, fragment, occurrence=0):
    highlighter = editor_module.HighlightingDecorator(source)
    styles = highlighter.highlight(Path(filename))
    start = -1
    for _ in range(occurrence + 1):
        start = source.index(fragment, start + 1)
    row = source.count("\n", 0, start)
    column = start - source.rfind("\n", 0, start) - 1
    return styles[row][column:column + len(fragment)]


@pytest.mark.parametrize("filename, source, fragment, category", [
    ("x.py", "from os import path\nif True: pass", "from", "preproc"),
    ("x.py", "from os import path\nif True: pass", "if", "statement"),
    ("x.py", "class Demo:\n    pass", "class", "statement"),
    ("x.py", "class Demo:\n    pass", "Demo", "type"),
    ("x.py", "raise ValueError('bad')", "ValueError", "type"),
    ("x.py", "def hello(): return 123", "hello", "identifier"),
    ("x.py", "def hello(): return 123", "123", "constant"),
    ("x.c", "#include <stdio.h>\nint main(void) { return 0; }", "include", "preproc"),
    ("x.c", "#include <stdio.h>\nint main(void) { return 0; }", "int", "type"),
    ("x.c", "int main(void) { return 0; }", "return", "statement"),
    ("x.c", "size_t length;", "size_t", "type"),
    ("x.cpp", "class Thing { public: void f() { if (true) return; } };", "class", "type"),
    ("x.cpp", "class Thing { public: void f() { if (true) return; } };", "public", "statement"),
    ("x.cpp", "constexpr int value = 1;", "constexpr", "type"),
    ("x.java", "package demo; public class Main { void f() { if (true) return; } }", "package", "preproc"),
    ("x.java", "package demo; public class Main { void f() { if (true) return; } }", "public", "type"),
    ("x.java", "class Main { void f() { if (true) return; } }", "if", "statement"),
    ("x.cs", "using System; public class Main { string s; }", "using", "statement"),
    ("x.cs", "using System; public class Main { string s; }", "public", "type"),
    ("x.cs", "using System; public class Main { string s; }", "string", "type"),
    ("x.cs", "#nullable enable\nclass Main {}", "nullable", "preproc"),
    ("x.py", "@staticmethod\ndef f(): pass", "@", "preproc"),
    ("x.py", "@staticmethod\ndef f(): pass", "staticmethod", "identifier"),
    ("x.java", "@Override void f() {}", "@Override", "preproc"),
    ("x.PY", "if True: pass", "if", "statement"),
])
def test_syntax_categories_for_all_five_languages(filename, source, fragment, category):
    assert source_category(source, filename, fragment) == [category] * len(fragment)


@pytest.mark.parametrize("filename, source, fragment, category", [
    ("x.py", 'if_count = "if # text" # return', "if_count", "normal"),
    ("x.py", 'if_count = "if # text" # return', "if # text", "constant"),
    ("x.py", 'if_count = "if # text" # return', "# return", "comment"),
    ("x.py", 's = """if\nwhile\nreturn"""\nif True: pass', "while", "constant"),
    ("x.c", "/* if\nwhile */\nif (1) return;", "while", "comment"),
    ("x.c", "/* if\nwhile */\nif (1) return;", "return", "statement"),
    ("x.java", "// if return\nclass Main {}", "if return", "comment"),
    ("x.cs", "string s = \"// not a comment\";", "not a comment", "constant"),
    ("x.cpp", "/* TODO: fix */", "TODO", "todo"),
    ("x.java", "/** @param value */", "@param", "special"),
])
def test_syntax_respects_strings_comments_and_word_boundaries(filename, source, fragment, category):
    assert source_category(source, filename, fragment) == [category] * len(fragment)


@pytest.mark.parametrize("filename, source, fragment", [
    ("x.py", r's = "hello\n"', r"\n"),
    ("x.py", r's = "\u0041"', r"\u0041"),
    ("x.c", r'char *s = "hello\n";', r"\n"),
    ("x.cpp", r'const char *s = "hello\n";', r"\n"),
    ("x.cs", r'string s = "hello\n";', r"\n"),
    ("x.cs", r'string s = "\U00000041";', r"\U00000041"),
    ("x.java", r'String s = "hello\n";', r"\n"),
    ("x.java", r'String s = "\u0041";', r"\u0041"),
    ("x.c", 'printf("%04lld %%");', "%04lld"),
    ("x.c", 'printf("%04lld %%");', "%%"),
    ("x.py", 's = f"{value!r}"', "{value!r}"[0]),
    ("x.cs", 'string s = $"{value,4:N2}";', "{"),
    ("x.cs", 'string s = $"{value,4:N2}";', ":N2"),
])
def test_variable_length_escapes_and_string_fields(filename, source, fragment):
    assert source_category(source, filename, fragment) == ["special"] * len(fragment)


@pytest.mark.parametrize("filename, source, fragment", [
    ("x.py", r's = r"raw\n"', r"\n"),
    ("x.cpp", r'auto s = R"tag(raw\n %d)tag";', r"\n %d"),
    ("x.cs", r'string s = @"C:\temp\new";', r"\temp\new"),
    ("x.cs", 'string s = """C:\\temp\\new""";', r"\temp\new"),
])
def test_raw_and_verbatim_strings_do_not_interpret_escapes(filename, source, fragment):
    assert source_category(source, filename, fragment) == ["constant"] * len(fragment)


def test_unsupported_extension_is_plain_text():
    source = 'if int #include "string" 123'
    styles = editor_module.HighlightingDecorator(source).highlight(Path("notes.txt"))
    assert styles == [["normal"] * len(source)]


def test_source_offsets_and_trailing_empty_lines_are_preserved():
    source = '\tif True:\n    print("x\\n")\n\n'
    styles = editor_module.HighlightingDecorator(source).highlight(Path("x.py"))
    assert list(map(len, styles)) == [len(line) for line in source.split("\n")]
    assert styles[-2:] == [[], []]


def test_sy_dispatch_toggle_and_undo():
    editor = Editor(Document("if True: pass"))
    assert not editor.syntax_enabled
    type_command(editor, "sy", "ENTER")
    assert editor.syntax_enabled
    assert isinstance(editor.history.commands(editor)[-1], editor_module.SyntaxCommand)
    assert editor.message == "Syntax highlighting on"
    type_command(editor, "sy", "ENTER")
    assert not editor.syntax_enabled
    assert editor.message == "Syntax highlighting off"
    editor.undo()
    assert editor.syntax_enabled


def test_sy_does_not_modify_document_or_cursor():
    editor = Editor(Document("abc\ndef"))
    editor.row, editor.column = 1, 2
    type_command(editor, "sy", "ENTER")
    assert editor.document.text() == "abc\ndef"
    assert not editor.document.modified
    assert (editor.row, editor.column) == (1, 2)


def test_sy_reports_missing_optional_lexer_without_entering_history(monkeypatch):
    editor = Editor(Document())
    monkeypatch.setattr(editor_module, "get_lexer_by_name", None)
    type_command(editor, "sy", "ENTER")
    assert not editor.syntax_enabled
    assert "Pygments" in editor.message
    assert not any(isinstance(command, editor_module.SyntaxCommand) for command in editor.history.commands(editor))


def test_sy_rejects_unexpected_argument():
    editor = Editor(Document())
    type_command(editor, "sy invalid", "ENTER")
    assert not editor.syntax_enabled
    assert "Unknown command" in editor.message


def test_curses_palette_uses_requested_colors(fake_curses):
    view = TerminalView(FakeWindow())
    assert fake_curses.pairs[1] == (11, -1)  # yellow
    assert fake_curses.pairs[2] == (10, -1)  # light green
    assert fake_curses.pairs[3] == (12, -1)  # light blue
    assert fake_curses.pairs[4] == (9, -1)   # light red
    assert view.styles["type"] == fake_curses.color_pair(2)


def test_eight_color_terminal_uses_bold_for_bright_colors(fake_curses):
    fake_curses.COLORS = 8
    view = TerminalView(FakeWindow())
    assert fake_curses.pairs[2][0] == fake_curses.COLOR_GREEN
    assert fake_curses.pairs[3][0] == fake_curses.COLOR_BLUE
    assert fake_curses.pairs[4][0] == fake_curses.COLOR_RED
    for category in ("type", "preproc", "special"):
        assert view.styles[category] & fake_curses.A_BOLD


def test_colorless_terminal_remains_usable(fake_curses, monkeypatch):
    monkeypatch.setattr(fake_curses, "has_colors", lambda: False)
    screen = FakeWindow(5, 30)
    view = TerminalView(screen)
    editor = Editor(Document("if True: pass"), view)
    editor.syntax_enabled = True
    view.draw(editor)
    assert all(attribute == 0 for attribute in screen.attributes[0])
    assert screen.row_text(0) == "if True: pass"


def test_syntax_search_and_gutter_attributes_are_combined(fake_curses):
    screen = FakeWindow(4, 12)
    view = TerminalView(screen)
    view.show_num = True
    editor = Editor(Document("\tif True: pass"), view)
    editor.syntax_enabled = True
    editor.column = len(str(editor.document.lines[0])) - 1
    editor.search_match = (0, 1, 2)
    view.draw(editor)
    assert screen.row_text(0).startswith("1 ")
    # A wide redraw brings the searched keyword back on screen.
    screen.width = 40
    editor.column = 1
    view.draw(editor)
    assert screen.row_text(0) == "1     if True: pass"
    assert screen.attributes[0][6:8] == [view.styles["statement"] | fake_curses.A_REVERSE] * 2
    assert screen.attributes[0][:2] == [0, 0]
    assert screen.cursor == (0, 6)


def test_syntax_disabled_draws_plain_text_then_colors_after_sy(fake_curses):
    screen = FakeWindow(6, 40)
    view = TerminalView(screen)
    editor = Editor(Document('if True: print("hello")'), view)
    view.draw(editor)
    assert screen.attributes[0][:2] == [0, 0]
    type_command(editor, "sy", "ENTER")
    view.draw(editor)
    assert screen.attributes[0][:2] == [view.styles["statement"]] * 2
    assert "SY" in screen.row_text(4)
    type_command(editor, "sy", "ENTER")
    view.draw(editor)
    assert screen.attributes[0][:2] == [0, 0]


def test_highlighting_cache_invalidates_on_text_and_path_changes(fake_curses, monkeypatch):
    screen = FakeWindow(6, 40)
    view = TerminalView(screen)
    editor = Editor(Document("class Demo: pass"), view)
    editor.syntax_enabled = True
    calls = []
    original = view.highlighter.highlight
    def tracked(path):
        calls.append((view.highlighter.source, path))
        return original(path)
    monkeypatch.setattr(view.highlighter, "highlight", tracked)
    view.draw(editor)
    editor.column = 1
    view.draw(editor)
    assert len(calls) == 1
    editor.document.lines[0].append(" # edited")
    view.draw(editor)
    assert len(calls) == 2
    editor.document.path = Path("x.cpp")
    view.draw(editor)
    assert len(calls) == 3
    editor.undo()  # Empty history must not disturb the cache.


@pytest.mark.parametrize("height,width", [(1, 1), (1, 8), (2, 2), (3, 1), (4, 8)])
def test_curses_handles_tiny_and_resized_windows(height, width, fake_curses):
    screen = FakeWindow(10, 60)
    view = TerminalView(screen)
    view.show_num = True
    editor = Editor(Document("abcdef\nsecond\nthird"), view)
    editor.row, editor.column = 2, 4
    editor.syntax_enabled = True
    view.draw(editor)
    screen.height, screen.width = height, width
    view.draw(editor)
    assert 0 <= screen.cursor[0] < height
    assert 0 <= screen.cursor[1] < width


@pytest.mark.parametrize("key, expected", [
    ("a", "a"), ("\x1b", "ESC"), ("\n", "ENTER"), ("\r", "ENTER"),
    ("\b", "BACKSPACE"), ("\x7f", "BACKSPACE"), ("\t", "TAB"),
    (FakeCurses.KEY_LEFT, "LEFT"), (FakeCurses.KEY_RIGHT, "RIGHT"),
    (FakeCurses.KEY_UP, "UP"), (FakeCurses.KEY_DOWN, "DOWN"),
    (FakeCurses.KEY_PPAGE, "PG_UP"), (FakeCurses.KEY_NPAGE, "PG_DN"),
    (FakeCurses.KEY_DC, "DELETE"), (FakeCurses.KEY_BACKSPACE, "BACKSPACE"),
    (FakeCurses.KEY_ENTER, "ENTER"), (FakeCurses.KEY_RESIZE, "RESIZE"),
    (12345, "UNKNOWN"), ("\x04", "PG_DN"), ("\x15", "PG_UP"),
])
def test_curses_key_translation(key, expected, fake_curses):
    screen = FakeWindow(keys=[key])
    terminal = editor_module.CursesTerminal(screen)
    assert screen.keypad_enabled
    assert terminal.read_key() == expected


def test_page_commands_use_actual_curses_window_height(fake_curses):
    screen = FakeWindow(7, 30)
    view = TerminalView(screen)
    editor = Editor(Document("\n".join(["line"] * 40)), view)
    press(editor, "PG_DN")
    assert editor.row == 5
    screen.height = 10
    press(editor, "PG_DN")
    assert editor.row == 13
    press(editor, "PG_UP")
    assert editor.row == 5


def test_curses_event_loop_handles_resize_sy_save_and_exit(fake_curses, tmp_path):
    target = tmp_path / "file.py"
    keys = [FakeCurses.KEY_RESIZE, ":", "s", "y", "\n", "i", "X", "\x1b",
            ":", *f"wq {target}", "\n"]
    screen = FakeWindow(8, 50, keys)
    document = Document("print('hello')")
    editor_module.run_editor(screen, document)
    assert target.read_text(encoding="ascii") == "Xprint('hello')"
    assert not screen.keys
    assert screen.refreshes > 5


def test_main_uses_curses_wrapper_for_terminal_cleanup(fake_curses, monkeypatch):
    calls = []
    def wrapper(callback, document):
        calls.append((callback, document))
        callback(FakeWindow(keys=[":", "q", "!", "\n"]), document)
    monkeypatch.setattr(fake_curses, "wrapper", wrapper, raising=False)
    monkeypatch.setattr(editor_module.sys, "argv", ["MyVim.py"])
    monkeypatch.setattr(editor_module.sys.stdin, "isatty", lambda: True)
    monkeypatch.setattr(editor_module.sys.stdout, "isatty", lambda: True)
    assert editor_module.main() == 0
    assert calls[0][0] is editor_module.run_editor


def test_main_has_actionable_error_when_curses_is_missing(monkeypatch, capsys):
    monkeypatch.setattr(editor_module, "curses", None)
    monkeypatch.setattr(editor_module.sys, "argv", ["MyVim.py"])
    assert editor_module.main() == 1
    assert "windows-curses" in capsys.readouterr().err


def test_main_refuses_redirected_input(fake_curses, monkeypatch, capsys):
    monkeypatch.setattr(editor_module.sys, "argv", ["MyVim.py"])
    monkeypatch.setattr(editor_module.sys.stdin, "isatty", lambda: False)
    assert editor_module.main() == 1
    assert "interactive terminal" in capsys.readouterr().err



def test_curses_colors_use_pdcurses_constants_not_ansi_indices(fake_curses):
    # windows-curses/PDCurses orders its base colours like the Windows console.
    fake_curses.COLOR_RED = 4
    fake_curses.COLOR_YELLOW = 6
    fake_curses.COLOR_BLUE = 1
    fake_curses.COLOR_CYAN = 3
    TerminalView(FakeWindow())
    assert fake_curses.pairs[1][0] == 14  # bright yellow, not bright cyan
    assert fake_curses.pairs[2][0] == 10
    assert fake_curses.pairs[3][0] == 9   # bright blue, not bright red
    assert fake_curses.pairs[4][0] == 12  # bright red, not bright blue



def test_python_type_alias_and_type_builtin_have_different_colors():
    source = "type Alias = int\nvalue = type(1)"
    assert source_category(source, "x.py", "type", 0) == ["statement"] * 4
    assert source_category(source, "x.py", "Alias") == ["type"] * 5
    assert source_category(source, "x.py", "type", 1) == ["identifier"] * 4


def test_curses_sanitizes_status_and_message_output(fake_curses):
    screen = FakeWindow(5, 40)
    view = TerminalView(screen)
    editor = Editor(Document("abc"), view)
    editor.message = "a\x1b[2J\nb"
    view.draw(editor)
    assert screen.row_text(4) == "a?[2J?b"


# ---------------------------------------------------------------------------
# Singleton History and keyboard u / U / :e!
# ---------------------------------------------------------------------------


def test_history_construction_is_singleton_without_resetting_stacks():
    history = editor_module.History()
    editor = Editor(Document("abc"))
    press(editor, "x")
    commands, changes = history.commands(editor), history.changes(editor)
    assert editor_module.History() is history
    assert editor_module.History.__new__(editor_module.History) is history
    assert editor.history is history
    assert history.commands(editor) == commands
    assert history.changes(editor) == changes
    assert isinstance(commands, tuple) and isinstance(changes, tuple)
    assert changes[-1].before == ("abc",)
    assert changes[-1].after == ("bc",)


def test_history_copy_deepcopy_and_pickle_cannot_duplicate_instance():
    import copy
    import pickle
    history = editor_module.History()
    editor = Editor(Document("abc"))
    press(editor, "x")
    before = history.changes(editor)
    assert copy.copy(history) is history
    assert copy.deepcopy(history) is history
    for protocol in range(pickle.HIGHEST_PROTOCOL + 1):
        assert pickle.loads(pickle.dumps(history, protocol=protocol)) is history
    assert history.changes(editor) == before


def test_history_is_singleton_across_threads():
    from concurrent.futures import ThreadPoolExecutor
    with ThreadPoolExecutor(max_workers=16) as pool:
        instances = list(pool.map(lambda _: editor_module.History(), range(128)))
    assert all(instance is instances[0] for instance in instances)


def test_history_subclassing_is_forbidden():
    with pytest.raises(TypeError, match="singleton"):
        type("DuplicateHistory", (editor_module.History,), {})


def test_singleton_keeps_editors_isolated():
    first = Editor(Document("first"))
    second = Editor(Document("second"))
    assert first.history is second.history
    press(first, "x")
    press(second, "x")
    press(first, "u")
    assert first.document.text() == "first"
    assert second.document.text() == "econd"
    assert not first.history.changes(first)
    assert len(second.history.changes(second)) == 1


def test_singleton_does_not_keep_dead_editor_alive():
    import gc
    import weakref
    editor = Editor(Document("abc"))
    press(editor, "x")
    reference = weakref.ref(editor)
    del editor
    gc.collect()
    assert reference() is None


def test_u_undoes_text_not_movement_copy_or_settings():
    view = TerminalView()
    editor = Editor(Document("abcdef\nsecond"), view)
    press(editor, "x", "RIGHT", "DOWN", "y", "y")
    type_command(editor, "set num", "ENTER")
    type_command(editor, "sy", "ENTER")
    copied = str(editor.document.buffer)
    press(editor, "u")
    assert editor.document.text() == "abcdef\nsecond"
    assert not editor.document.modified
    assert editor.mode == "NORMAL"
    assert editor.syntax_enabled
    assert view.show_num
    assert str(editor.document.buffer) == copied
    assert (editor.row, editor.column) == (0, 0)
    assert not editor.history.changes(editor)


def test_u_repeatedly_undoes_individual_insert_commands():
    editor = Editor(Document("abc"))
    press(editor, "i", "X", "Y", "ESC")
    assert editor.document.text() == "XYabc"
    press(editor, "u")
    assert editor.document.text() == "Xabc"
    assert editor.document.modified
    assert editor.mode == "NORMAL"
    press(editor, "u")
    assert editor.document.text() == "abc"
    assert not editor.document.modified
    press(editor, "u")
    assert editor.document.text() == "abc"
    assert "No changes" in editor.message


@pytest.mark.parametrize("keys, original", [
    (("x",), "abc"),
    (("r", "Z"), "abc"),
    (("d", "w"), "abc def"),
    (("d", "i", "w"), "abc def"),
    (("d", "d"), "one\ntwo"),
    (("S", "X", "ESC"), "one\ntwo"),
    (("i", "ENTER", "ESC"), "abc"),
    (("i", "RIGHT", "BACKSPACE", "ESC"), "abc"),
    (("y", "y", "p"), "one\ntwo"),
    (("y", "w", "p"), "hello world"),
])
def test_u_can_restore_each_edit_kind(keys, original):
    editor = Editor(Document(original))
    press(editor, *keys)
    for _ in range(len(editor.history.changes(editor))):
        press(editor, "u")
    assert editor.document.text() == original
    assert not editor.document.modified
    assert editor.mode == "NORMAL"


def test_u_undoes_line_merge():
    editor = Editor(Document("one\ntwo\nthree"))
    press(editor, "DOWN", "i", "BACKSPACE", "ESC")
    assert editor.document.text() == "onetwo\nthree"
    press(editor, "u")
    assert editor.document.text() == "one\ntwo\nthree"
    assert (editor.row, editor.column) == (1, 0)


def test_noop_changes_and_yank_do_not_pollute_text_history():
    editor = Editor(Document())
    press(editor, "x", "d", "d", "y", "y", "S", "ESC")
    assert not editor.history.changes(editor)
    assert not editor.document.modified
    editor = Editor(Document("abc"))
    press(editor, "r", "a", "y", "w")
    assert not editor.history.changes(editor)
    assert not editor.document.modified


def test_U_restores_only_current_line_and_u_can_undo_U():
    editor = Editor(Document("first\nsecond"))
    press(editor, "i", "X", "Y", "ESC", "DOWN", "0", "i", "Z", "ESC")
    assert editor.document.text() == "XYfirst\nZsecond"
    press(editor, "UP", "U")
    assert editor.document.text() == "first\nZsecond"
    assert editor.document.modified
    assert isinstance(editor.history.commands(editor)[-1], editor_module.UndoLineCommand)
    press(editor, "u")
    assert editor.document.text() == "XYfirst\nZsecond"
    assert editor.mode == "NORMAL"


def test_U_returns_clean_flag_when_only_one_line_was_changed():
    editor = Editor(Document("first\nsecond"))
    press(editor, "i", "X", "Y", "ESC", "U")
    assert editor.document.text() == "first\nsecond"
    assert not editor.document.modified
    count = len(editor.history.changes(editor))
    press(editor, "U")
    assert len(editor.history.changes(editor)) == count
    assert "no changes" in editor.message


def test_U_tracks_line_identity_when_an_earlier_line_is_deleted():
    editor = Editor(Document("same\nsame\nsame"))
    press(editor, "DOWN", "i", "X", "ESC", "UP", "d", "d")
    assert editor.document.text() == "Xsame\nsame"
    press(editor, "U")
    assert editor.document.text() == "same\nsame"


def test_U_tracks_line_identity_when_an_earlier_line_is_inserted():
    editor = Editor(Document("first\nsecond"))
    press(editor, "DOWN", "i", "X", "ESC", "UP", "y", "y", "p", "DOWN", "U")
    assert editor.document.text() == "first\nfirst\nsecond"


def test_U_on_new_line_uses_its_initial_contents():
    editor = Editor(Document("abcd"))
    press(editor, "RIGHT", "RIGHT", "i", "ENTER", "X", "ESC")
    assert editor.document.text() == "ab\nXcd"
    press(editor, "U")
    assert editor.document.text() == "ab\ncd"
    press(editor, "u", "u", "u")
    assert editor.document.text() == "abcd"
    press(editor, "U")
    assert editor.document.text() == "abcd"


def test_u_keeps_savepoint_and_does_not_undo_disk_write(tmp_path):
    path = tmp_path / "saved.txt"
    path.write_text("abc", encoding="ascii")
    editor = Editor(Document.open_file(path))
    press(editor, "x")
    type_command(editor, "w", "ENTER")
    assert path.read_text(encoding="ascii") == "bc"
    assert not editor.document.modified
    press(editor, "u")
    assert editor.document.text() == "abc"
    assert editor.document.modified
    assert path.read_text(encoding="ascii") == "bc"
    press(editor, "U")
    assert editor.document.text() == "bc"
    assert not editor.document.modified


def test_U_uses_new_saved_line_checkpoint(tmp_path):
    path = tmp_path / "saved.txt"
    editor = Editor(Document("abc"))
    press(editor, "x")
    type_command(editor, f"w {path}", "ENTER")
    press(editor, "i", "X", "ESC", "U")
    assert editor.document.text() == "bc"
    assert not editor.document.modified


def test_history_recognises_direct_Document_save(tmp_path):
    editor = Editor(Document("abc"))
    press(editor, "x")
    path = tmp_path / "direct.txt"
    editor.document.save(path)
    press(editor, "i", "X", "ESC", "U")
    assert editor.document.text() == "bc"
    assert not editor.document.modified


def test_e_force_reloads_saved_file_and_clears_only_its_stacks(tmp_path):
    first_path = tmp_path / "first.txt"
    second_path = tmp_path / "second.txt"
    first_path.write_bytes(b"one\r\ntwo\r\n")
    second_path.write_bytes(b"second")
    first = Editor(Document.open_file(first_path), TerminalView())
    second = Editor(Document.open_file(second_path))
    press(first, "x", "DOWN", "i", "Z", "ESC")
    press(second, "x")
    type_command(first, "sy", "ENTER")
    first.document.buffer = MyString("clipboard")
    type_command(first, "e!", "ENTER")
    assert first.document.text() == "one\ntwo\n"
    assert first.document.newline == "\r\n"
    assert not first.document.modified
    assert first.syntax_enabled
    assert str(first.document.buffer) == "clipboard"
    assert not first.history.changes(first)
    assert not first.history.commands(first)
    assert second.document.text() == "econd"
    assert len(second.history.changes(second)) == 1
    assert first_path.read_bytes() == b"one\r\ntwo\r\n"
    press(first, "u")
    assert first.document.text() == "one\ntwo\n"


def test_e_force_uses_latest_external_disk_content(tmp_path):
    path = tmp_path / "external.txt"
    path.write_bytes(b"old")
    editor = Editor(Document.open_file(path))
    press(editor, "x")
    path.write_bytes(b"external\r\nchange")
    type_command(editor, "e!", "ENTER")
    assert editor.document.text() == "external\nchange"
    assert editor.document.newline == "\r\n"
    assert not editor.document.modified


def test_e_force_preserves_edits_and_history_on_invalid_file(tmp_path):
    path = tmp_path / "bad.txt"
    path.write_bytes(b"valid")
    editor = Editor(Document.open_file(path))
    press(editor, "x")
    changes = editor.history.changes(editor)
    path.write_bytes(b"not ascii: \xff")
    type_command(editor, "e!", "ENTER")
    assert editor.document.text() == "alid"
    assert editor.document.modified
    assert editor.history.changes(editor) == changes
    assert "ascii" in editor.message


@pytest.mark.parametrize("initial", ["", "initial\ntext"])
def test_e_force_resets_unnamed_buffer_to_initial_state(initial):
    editor = Editor(Document(initial))
    press(editor, "i", "X", "ENTER", "Y", "ESC")
    type_command(editor, "e!", "ENTER")
    assert editor.document.text() == initial
    assert editor.document.path is None
    assert not editor.document.modified
    assert not editor.history.changes(editor)


def test_e_force_does_not_create_a_missing_file(tmp_path):
    path = tmp_path / "missing.txt"
    editor = Editor(Document.open_file(path))
    press(editor, "i", "X", "ESC")
    type_command(editor, "e!", "ENTER")
    assert editor.document.text() == ""
    assert not path.exists()
    assert not editor.document.modified


def test_history_keeps_original_buffer_after_open_and_programmatic_undo(tmp_path):
    path = tmp_path / "other.txt"
    path.write_bytes(b"other")
    editor = Editor(Document("first"))
    press(editor, "x")
    original = editor.document
    type_command(editor, f"o! {path}", "ENTER")
    press(editor, "u")
    assert editor.document.text() == "other"
    editor.undo()  # The latest recorded command is still ForceOpenFileCommand.
    assert editor.document is original
    press(editor, "ESC", "u")
    assert editor.document.text() == "first"


def test_undo_change_command_can_restore_its_own_history_snapshot():
    editor = Editor(Document("abc"))
    press(editor, "x")
    before = editor.history.checkpoint(editor)
    action = editor_module.UndoChangeCommand()
    action.execute(editor)
    assert editor.document.text() == "abc"
    action.undo(editor)
    assert editor.document.text() == "bc"
    assert editor.history.checkpoint(editor) == before


def test_reload_command_can_restore_its_own_history_snapshot():
    editor = Editor(Document("abc"))
    press(editor, "x")
    before = editor.history.checkpoint(editor)
    action = editor_module.ReloadFileCommand()
    action.execute(editor)
    assert editor.document.text() == "abc"
    action.undo(editor)
    assert editor.document.text() == "bc"
    assert editor.history.checkpoint(editor) == before


def test_u_and_U_in_insert_mode_are_literal_text():
    editor = Editor(Document())
    press(editor, "i", "u", "U", "ESC")
    assert editor.document.text() == "uU"


def test_curses_event_loop_supports_all_history_commands(fake_curses, tmp_path):
    path = tmp_path / "history.py"
    path.write_text("abc", encoding="ascii")
    keys = ["i", "X", "Y", "\x1b", "u", "U", "i", "Z", "\x1b",
            ":", "e", "!", "\n", ":", "q", "\n"]
    document = Document.open_file(path)
    editor_module.run_editor(FakeWindow(8, 40, keys), document)
    assert document.text() == "abc"
    assert not document.modified
    assert path.read_bytes() == b"abc"



def test_history_first_creation_is_singleton_in_fresh_process():
    import subprocess
    import sys
    code = """
from concurrent.futures import ThreadPoolExecutor
from threading import Barrier
from MyVim import History
assert History._instance is None
barrier = Barrier(32)
def construct(_):
    barrier.wait()
    return History()
with ThreadPoolExecutor(max_workers=32) as pool:
    instances = list(pool.map(construct, range(32)))
assert len({id(instance) for instance in instances}) == 1
assert instances[0] is History()
print('singleton confirmed')
"""
    result = subprocess.run([sys.executable, "-B", "-c", code],
                            cwd=Path(__file__).resolve().parent,
                            check=True, capture_output=True, text=True, timeout=30)
    assert result.stdout.strip() == "singleton confirmed"
