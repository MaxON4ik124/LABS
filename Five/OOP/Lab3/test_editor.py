import io
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import Five.OOP.Lab3.myVimm as editor_module
from Five.OOP.Lab3.myVimm import Document, Editor, TerminalView, safe_screen_text
from my_string import MyString


class DocumentTests(unittest.TestCase):
    def test_insert_delete_and_storage(self):
        document = Document()
        self.assertIsInstance(document.lines[0], MyString)
        document.insert(0, 0, "abc")
        document.delete_character(0, 1)
        self.assertEqual(document.text(), "ac")
        self.assertTrue(document.modified)
        document.delete_character(0, 2)
        self.assertEqual(document.text(), "ac")

    def test_split_and_merge(self):
        document = Document("hello")
        document.split_line(0, 2)
        self.assertEqual(document.text(), "he\nllo")
        self.assertEqual(document.backspace(1, 0), (0, 2))
        self.assertEqual(document.text(), "hello")

    def test_empty_lines(self):
        document = Document()
        document.split_line(0, 0)
        self.assertEqual(document.text(), "\n")
        document.backspace(1, 0)
        document.insert(0, 0, "x")
        document.delete_line(0)
        self.assertEqual(document.text(), "")
        self.assertEqual(len(document.lines), 1)
        self.assertEqual(document.backspace(0, 0), (0, 0))

    def test_file_round_trips(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "test.txt"
            for data in (b"", b"abc", b"abc\n", b"a\n\nb\n", b"a\r\nb\r\n", b"\t hi  "):
                with self.subTest(data=data):
                    path.write_bytes(data)
                    document = Document.open(path)
                    document.save()
                    self.assertEqual(path.read_bytes(), data)
                    self.assertFalse(document.modified)

    def test_new_file_and_failed_save(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "new.txt"
            document = Document.open(path)
            self.assertFalse(path.exists())
            document.insert(0, 0, "new")
            with self.assertRaises(OSError):
                document.save(Path(directory) / "missing" / "file.txt")
            self.assertTrue(document.modified)
            self.assertEqual(document.path, path.resolve())
            document.save()
            self.assertEqual(path.read_bytes(), b"new")

    def test_reject_unsupported_text(self):
        for text in ("Привет", "\x1b[2J", "\x00", "a\rb"):
            with self.subTest(text=text), self.assertRaises(ValueError):
                Document(text)
        document = Document()
        with self.assertRaises(ValueError):
            document.insert(0, 0, "\n")

    def test_mixed_newlines_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "mixed.txt"
            path.write_bytes(b"a\r\nb\n")
            with self.assertRaises(ValueError):
                Document.open(path)

    def test_copy_line_is_a_real_copy(self):
        document = Document("aaa\nbbb")
        document.copy_line(0)
        self.assertEqual(str(document.buffer), "aaa\n")
        # editing the source line must NOT mutate the buffer (no aliasing)
        document.lines[0].append("X")
        self.assertEqual(str(document.buffer), "aaa\n")

    def test_copy_line_on_single_line(self):
        document = Document("only")
        document.copy_line(0)
        self.assertEqual(str(document.buffer), "only\n")

    def test_copy_word(self):
        document = Document("hello world")
        document.copy_word(0, 0)
        self.assertEqual(str(document.buffer), "hello")
        document.copy_word(0, 6)
        self.assertEqual(str(document.buffer), "world")

    def test_paste_line_buffer_inserts_new_line_below(self):
        document = Document("one\ntwo")
        document.copy_line(0)
        row, column = document.paste(0, 0)
        self.assertEqual((row, column), (1, 0))
        self.assertEqual(document.text(), "one\none\ntwo")

    def test_paste_word_buffer_inserts_after_cursor(self):
        document = Document("hello world")
        document.copy_word(0, 0)
        row, column = document.paste(0, 0)
        self.assertEqual((row, column), (0, 6))
        self.assertEqual(document.text(), "hhelloello world")

    def test_paste_works_with_cursor_at_end_of_line(self):
        document = Document("ab")
        document.copy_word(0, 0)
        document.paste(0, 2)
        self.assertEqual(document.text(), "abab")


class ControllerTests(unittest.TestCase):
    def keys(self, editor, *keys):
        for key in keys:
            editor.handle_key(key)

    def test_page_scrolling(self):
        lines = "\n".join(f"line{i}" for i in range(1, 51))
        editor = Editor(Document(lines))
        with patch.object(editor_module.shutil, "get_terminal_size", return_value=(80, 12)):
            self.keys(editor, "PG_DN")
            self.assertEqual((editor.row, editor.column), (10, 0))
            self.keys(editor, "PG_DN")
            self.assertEqual(editor.row, 20)
            self.keys(editor, "PG_UP")
            self.assertEqual(editor.row, 10)
            self.keys(editor, "PG_UP", "PG_UP")
            self.assertEqual(editor.row, 0)
            self.keys(editor, "PG_DN", "PG_DN", "PG_DN", "PG_DN", "PG_DN")
            self.assertEqual(editor.row, 49)

    def test_insert_still_works_with_diw_command(self):
        editor = Editor(Document())
        # a lone "i" must still enter INSERT (regression after adding "diw")
        self.keys(editor, "i", "H", "i", "ESC")
        self.assertEqual(editor.document.text(), "Hi")
        self.assertEqual(editor.mode, "NORMAL")
        # "d","d" must still delete a line
        editor = Editor(Document("one\ntwo\nthree"))
        self.keys(editor, "d", "d")
        self.assertEqual(editor.document.text(), "two\nthree")
        # "d","i","w" must run delete_word and not enter INSERT
        editor = Editor(Document("abc def"))
        self.keys(editor, "d", "i", "w")
        self.assertEqual(editor.mode, "NORMAL")
        # ESC must clear a half-typed "di" sequence, then "i" must open INSERT
        editor = Editor(Document())
        self.keys(editor, "d", "i", "ESC", "i", "Z", "ESC")
        self.assertEqual(editor.document.text(), "Z")
        # a lone "w" must still move by word (not swallow as "dw")
        editor = Editor(Document("aaa bbb ccc"))
        self.keys(editor, "d", "ESC", "w")
        self.assertEqual(editor.row, 0)

    def test_insert_normal_delete(self):
        editor = Editor(Document())
        self.keys(editor, "i", "H", "i", "ESC")
        self.assertEqual(editor.document.text(), "Hi")
        self.assertEqual(editor.mode, "NORMAL")
        self.keys(editor, "0", "x")
        self.assertEqual(editor.document.text(), "i")

    def test_enter_backspace_and_append(self):
        editor = Editor(Document("abc"))
        self.keys(editor, "$", "a", "ENTER", "x", "BACKSPACE", "BACKSPACE", "ESC")
        self.assertEqual(editor.document.text(), "abc")
        self.assertEqual((editor.row, editor.column), (0, 2))

    def test_dd_and_cancel_pending_command(self):
        editor = Editor(Document("one\ntwo"))
        self.keys(editor, "d", "ESC", "d")
        self.assertEqual(editor.document.text(), "one\ntwo")
        self.keys(editor, "d")
        self.assertEqual(editor.document.text(), "two")
        self.keys(editor, "d", "d", "d", "d")
        self.assertEqual(editor.document.text(), "")

    def test_ng_moves_to_nth_line(self):
        editor = Editor(Document("one\ntwo\nthree\nfour\nfive"))
        self.keys(editor, "3", "G")
        self.assertEqual((editor.row, editor.column), (2, 0))
        self.keys(editor, "1", "G")
        self.assertEqual((editor.row, editor.column), (0, 0))
        self.keys(editor, "5", "G")
        self.assertEqual((editor.row, editor.column), (4, 0))

    def test_ng_beyond_last_line_goes_to_end(self):
        editor = Editor(Document("one\ntwo\nthree"))
        self.keys(editor, "9", "9", "G")
        self.assertEqual((editor.row, editor.column), (2, 0))

    def test_plain_g_goes_to_last_line(self):
        editor = Editor(Document("one\ntwo\nthree"))
        self.keys(editor, "G")
        self.assertEqual((editor.row, editor.column), (2, 0))

    def test_zero_alone_is_motion_not_count(self):
        editor = Editor(Document("abcdef"))
        self.keys(editor, "$")
        self.assertEqual(editor.column, 5)
        self.keys(editor, "0")
        self.assertEqual(editor.column, 0)
        self.keys(editor, "1", "0", "G")
        self.assertEqual(editor.row, 0)

    def test_count_discarded_before_other_command(self):
        editor = Editor(Document("one\ntwo\nthree"))
        self.keys(editor, "2", "0", "g", "g")
        self.assertEqual((editor.row, editor.column), (0, 0))

    def test_enter_in_insert_mode_splits_line(self):
        editor = Editor(Document("hello"))
        self.keys(editor, "i", "RIGHT", "RIGHT", "ENTER")
        self.assertEqual(editor.document.text(), "he\nllo")
        self.assertEqual((editor.row, editor.column), (1, 0))
        self.assertEqual(editor.preferred_column, 0)
        self.keys(editor, "ESC")
        self.assertEqual((editor.row, editor.column), (1, 0))

    def test_yy_p_pastes_line_below(self):
        editor = Editor(Document("one\ntwo"))
        self.keys(editor, "y", "y", "p")
        self.assertEqual(editor.document.text(), "one\none\ntwo")
        self.assertEqual((editor.row, editor.column), (1, 0))

    def test_yw_p_pastes_word(self):
        editor = Editor(Document("hello world"))
        self.keys(editor, "y", "w", "p")
        self.assertEqual(editor.document.text(), "hhelloello world")
        self.assertEqual((editor.row, editor.column), (0, 6))

    def test_w_still_moves_when_not_yanked(self):
        editor = Editor(Document("hello world foo"))
        self.keys(editor, "w")
        self.assertEqual(editor.column, 4)
        self.assertEqual(str(editor.document.buffer), "")

    def test_preferred_column_and_boundaries(self):
        editor = Editor(Document("abcdef\nx\nabcdef"))
        self.keys(editor, "$", "DOWN")
        self.assertEqual(editor.column, 0)
        self.keys(editor, "DOWN")
        self.assertEqual(editor.column, 5)
        self.keys(editor, "DOWN", "RIGHT")
        self.assertEqual((editor.row, editor.column), (2, 5))

    def test_hjkl_do_not_move_in_normal_mode(self):
        editor = Editor(Document("abc\ndef\nghi"))
        self.keys(editor, "DOWN", "RIGHT")
        for key in "hjkl":
            with self.subTest(key=key):
                self.keys(editor, key)
                self.assertEqual((editor.row, editor.column), (1, 1))
                self.assertEqual(editor.document.text(), "abc\ndef\nghi")

    def test_arrows_move_in_normal_and_insert_modes(self):
        for mode in ("NORMAL", "INSERT"):
            with self.subTest(mode=mode):
                editor = Editor(Document("abc\ndef"))
                editor.mode = mode
                self.keys(editor, "RIGHT", "DOWN")
                self.assertEqual((editor.row, editor.column), (1, 1))
                self.keys(editor, "LEFT", "UP")
                self.assertEqual((editor.row, editor.column), (0, 0))

    # def test_hjkl_are_inserted_as_text(self):
    #     editor = Editor(Document())
    #     self.keys(editor, "i", *"hjkl")
    #     self.assertEqual(editor.document.text(), "hjkl")
    #     self.assertEqual((editor.row, editor.column), (0, 4))

    def test_save_quit_and_command_cancel(self):
        with tempfile.TemporaryDirectory() as directory:
            editor = Editor(Document())
            self.keys(editor, "i", "x", "ESC", ":", "q", "ENTER")
            self.assertTrue(editor.running)
            self.assertIn("Unsaved", editor.message)
            path = Path(directory) / "file with spaces.txt"
            self.keys(editor, ":", *f"w {path}", "ENTER")
            self.assertEqual(path.read_bytes(), b"x")
            self.keys(editor, ":", "q", "ESC")
            self.assertTrue(editor.running)
            self.keys(editor, ":", "q", "ENTER")
            self.assertFalse(editor.running)

    def test_write_error_and_force_quit(self):
        editor = Editor(Document("abc"))
        self.keys(editor, ":", "w", "ENTER")
        self.assertIn("No file name", editor.message)
        self.keys(editor, "x", ":", "q", "!", "ENTER")
        self.assertFalse(editor.running)

    def test_write_and_quit(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "saved.txt"
            editor = Editor(Document("abc"))
            self.keys(editor, ":", *f"wq {path}", "ENTER")
            self.assertFalse(editor.running)
            self.assertEqual(path.read_bytes(), b"abc")

    def test_tab_and_non_ascii_input(self):
        editor = Editor(Document())
        self.keys(editor, "i", "TAB", "я")
        self.assertEqual(editor.document.text(), "    ")
        self.assertEqual(editor.column, 4)
        self.assertIn("ASCII", editor.message)


class ViewTests(unittest.TestCase):
    def test_scrolling_and_safe_output(self):
        editor = Editor(Document("\t0123456789\nsecond\nthird"))
        editor.column = 10
        view = TerminalView()
        with patch("editor.shutil.get_terminal_size", return_value=(10, 4)):
            with patch("sys.stdout", new_callable=io.StringIO) as output:
                view.draw(editor)
                self.assertGreater(view.left, 0)
                self.assertIn("NORMAL", output.getvalue())
            editor.row, editor.column = 2, 0
            with patch("sys.stdout", new_callable=io.StringIO):
                view.draw(editor)
                self.assertEqual(view.top, 1)
        self.assertEqual(safe_screen_text("a\x1b\nb"), "a??b")


if __name__ == "__main__":
    unittest.main()