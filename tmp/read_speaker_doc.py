# -*- coding: utf-8 -*-
"""Показать содержимое образца текста для двух докладчиков."""
import os

from docx import Document

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LAW = os.path.join(BASE, "Five", "Law")


def main():
    path = os.path.join(LAW, "Bibliv_Mikhailich_2_text_for_two_speakers.docx")
    doc = Document(path)
    print("PARAGRAPHS:", len(doc.paragraphs))
    total_words = 0
    for i, p in enumerate(doc.paragraphs):
        text = p.text.strip()
        if not text:
            continue
        total_words += len(text.split())
        # Печатаем только маркеры рассказчиков и заголовки слайдов
        low = text.lower()
        if low.startswith("рассказчик") or low.startswith("слайд") or text.endswith("!") and len(text) < 60:
            print(f"[{i}] {text}")
    print("ИТОГО СЛОВ:", total_words, "≈ минут:", round(total_words / 130, 1))


if __name__ == "__main__":
    main()