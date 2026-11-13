from pathlib import Path
import re

import pytest

import MyVim as editor_module
from MyVim import Document, Editor, TerminalView, safe_screen_text
from my_string import MyString


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


def test_scrolling_and_safe_output(monkeypatch: pytest.MonkeyPatch, capsys):
    monkeypatch.setattr(
        editor_module.shutil, "get_terminal_size", lambda *a, **k: (10, 4)
    )
    editor = Editor(Document("\t0123456789\nsecond\nthird"))
    editor.column = 10
    view = TerminalView()

    view.draw(editor)
    assert view.left > 0
    assert "NORMAL" in capsys.readouterr().out

    editor.row, editor.column = 2, 0
    view.draw(editor)
    assert view.top == 1

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
    assert type(editor.history[-1]).__name__ == command_name


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
    assert type(editor.history[-1]).__name__ == command_name


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
                   for action in editor.history)
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
    assert isinstance(editor.history[-1], editor_module.ConfirmSearchCommand)
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
    assert not editor.history


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
def test_numbered_view_cursor_reaches_every_character(mode, text, monkeypatch, capsys):
    monkeypatch.setattr(editor_module.shutil, "get_terminal_size", lambda *a: (12, 4))
    view = TerminalView()
    view.show_num = True
    editor = Editor(Document(text), view)
    editor.mode = mode
    expanded = text.expandtabs(4)
    limit = len(text) if mode == "INSERT" else len(text) - 1
    for column in range(limit + 1):
        editor.column = column
        view.draw(editor)
        output = capsys.readouterr().out
        body_line = output.split("\x1b[1;1H\x1b[2K", 1)[1].split("\x1b[2;1H", 1)[0]
        cursor = re.search(r"\x1b\[(\d+);(\d+)H\x1b\[\?25h$", output)
        assert cursor is not None
        cursor_row, cursor_column = map(int, cursor.groups())
        screen_column = len(text[:column].expandtabs(4))
        assert body_line == "1 " + expanded[view.left:view.left + 9]
        assert (cursor_row, cursor_column) == (1, 2 + screen_column - view.left + 1)
        assert 3 <= cursor_column <= 11
        if column < len(text):
            assert body_line[cursor_column - 1] == expanded[screen_column]
        else:
            assert cursor_column == len(body_line) + 1


def test_number_gutter_uses_document_rows_after_scrolling(monkeypatch, capsys):
    monkeypatch.setattr(editor_module.shutil, "get_terminal_size", lambda *a: (12, 4))
    view = TerminalView()
    view.show_num = True
    editor = Editor(Document("\n".join(["abcdefghijklmnop"] * 12)), view)
    editor.row, editor.column = 11, 15
    view.draw(editor)
    output = capsys.readouterr().out
    assert view.top == 10
    assert view.left == 8
    assert "\x1b[1;1H\x1b[2K11 ijklmnop" in output
    assert "\x1b[2;1H\x1b[2K12 ijklmnop" in output
    assert output.endswith("\x1b[2;11H\x1b[?25h")


def test_number_gutter_does_not_shift_search_highlight(monkeypatch, capsys):
    monkeypatch.setattr(editor_module.shutil, "get_terminal_size", lambda *a: (12, 4))
    view = TerminalView()
    view.show_num = True
    editor = Editor(Document("abcdefghijklmnop"), view)
    editor.column = 15
    editor.search_match = (0, 13, 3)
    view.draw(editor)
    output = capsys.readouterr().out
    assert "\x1b[1;1H\x1b[2K1 hijklm\x1b[7mnop\x1b[0m" in output
    assert output.endswith("\x1b[1;11H\x1b[?25h")


@pytest.mark.parametrize("mode, command", [("COMMAND", "set num"), ("SEARCH", "abc")])
def test_number_gutter_does_not_shift_prompt_cursor(mode, command, monkeypatch, capsys):
    monkeypatch.setattr(editor_module.shutil, "get_terminal_size", lambda *a: (20, 4))
    view = TerminalView()
    view.show_num = True
    editor = Editor(Document("abc"), view)
    editor.mode, editor.command = mode, command
    view.draw(editor)
    output = capsys.readouterr().out
    assert output.endswith(f"\x1b[4;{len(command) + 2}H\x1b[?25h")


@pytest.mark.parametrize("terminal_width", [1, 2, 3])
def test_number_gutter_preserves_text_space_in_narrow_terminal(terminal_width, monkeypatch, capsys):
    monkeypatch.setattr(editor_module.shutil, "get_terminal_size",
                        lambda *a: (terminal_width, 4))
    view = TerminalView()
    view.show_num = True
    editor = Editor(Document("abcdef"), view)
    editor.column = 5
    view.draw(editor)
    output = capsys.readouterr().out
    width = max(1, terminal_width - 1)
    assert output.endswith(f"\x1b[1;{width}H\x1b[?25h")
    assert "f" in output.split("\x1b[2;1H", 1)[0]


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
