# -*- coding: utf-8 -*-
"""Проверить итоговый документ с текстом выступления."""
import os

from docx import Document

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PATH = os.path.join(BASE, "Five", "Law",
                    "Антивирус_вариант_7_KES_текст_2_рассказчика.docx")


def main():
    doc = Document(PATH)
    words = 0
    for p in doc.paragraphs:
        t = p.text.strip()
        if not t:
            continue
        words += len(t.split())
        bold = any(r.bold for r in p.runs if r.text.strip())
        mark = "##" if bold else "  "
        print(f"{mark} {t[:110]}")
    print("-" * 60)
    print("ПАРАГРАФОВ:", len([p for p in doc.paragraphs if p.text.strip()]))
    print("СЛОВ:", words, "≈ минут:", round(words / 130, 1))


if __name__ == "__main__":
    main()