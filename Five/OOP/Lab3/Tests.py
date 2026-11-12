from pathlib import Path

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
    assert isinstance(document.lines[0], MyString)

    document.insert(0, 0, "abc")
    document.delete_character(0, 1)
    assert document.text() == "ac"
    assert document.modified is True

    document.delete_character(0, 2)
    assert document.text() == "ac"


def test_split_and_merge():
    document = Document("hello")
    document.split_line(0, 2)
    assert document.text() == "he\nllo"
    assert document.backspace(1, 0) == (0, 2)
    assert document.text() == "hello"


def test_empty_lines():
    document = Document()
    document.split_line(0, 0)
    assert document.text() == "\n"

    document.backspace(1, 0)
    document.insert(0, 0, "x")
    document.delete_line(0)
    assert document.text() == ""
    assert len(document.lines) == 1
    assert document.backspace(0, 0) == (0, 0)


@pytest.mark.parametrize(
    "data",
    [b"", b"abc", b"abc\n", b"a\n\nb\n", b"a\r\nb\r\n", b"\t hi  "],
    ids=["empty", "no-newline", "trailing-lf", "blank-line", "crlf", "tabs-spaces"],
)
def test_file_round_trips(tmp_path: Path, data: bytes):
    path = tmp_path / "test.txt"
    path.write_bytes(data)

    document = Document.open(path)
    document.save()

    assert path.read_bytes() == data
    assert document.modified is False


def test_new_file_and_failed_save(tmp_path: Path):
    path = tmp_path / "new.txt"
    document = Document.open(path)
    assert path.exists() is False

    document.insert(0, 0, "new")
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
        document.insert(0, 0, "\n")


def test_mixed_newlines_rejected(tmp_path: Path):
    path = tmp_path / "mixed.txt"
    path.write_bytes(b"a\r\nb\n")
    with pytest.raises(ValueError):
        Document.open(path)


def test_copy_line_is_a_real_copy():
    document = Document("aaa\nbbb")
    document.copy_line(0)
    assert str(document.buffer) == "aaa\n"

    document.lines[0].append("X")
    assert str(document.buffer) == "aaa\n"


def test_copy_line_on_single_line():
    document = Document("only")
    document.copy_line(0)
    assert str(document.buffer) == "only\n"


def test_copy_word():
    document = Document("hello world")
    document.copy_word(0, 0)
    assert str(document.buffer) == "hello"

    document.copy_word(0, 6)
    assert str(document.buffer) == "world"


def test_paste_line_buffer_inserts_new_line_below():
    document = Document("one\ntwo")
    document.copy_line(0)
    assert document.paste(0, 0) == (1, 0)
    assert document.text() == "one\none\ntwo"


def test_paste_word_buffer_inserts_after_cursor():
    document = Document("hello world")
    document.copy_word(0, 0)
    assert document.paste(0, 0) == (0, 6)
    assert document.text() == "hhelloello world"


def test_paste_works_with_cursor_at_end_of_line():
    document = Document("ab")
    document.copy_word(0, 0)
    document.paste(0, 2)
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