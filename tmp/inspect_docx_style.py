# -*- coding: utf-8 -*-
"""Показать настройки образца текста для двух докладчиков (шрифты, размеры)."""
import os

from docx import Document
from docx.shared import Pt

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LAW = os.path.join(BASE, "Five", "Law")


def main():
    path = os.path.join(LAW, "Bibliv_Mikhailich_2_text_for_two_speakers.docx")
    doc = Document(path)
    normal = doc.styles["Normal"]
    print("Normal font:", normal.font.name, normal.font.size)
    sec = doc.sections[0]
    print("page:", sec.page_width, sec.page_height, "margins:", sec.left_margin, sec.right_margin)
    for i, p in enumerate(doc.paragraphs[:6]):
        runs = [(r.text[:30], r.font.name, r.font.size, r.font.bold) for r in p.runs]
        print(f"[{i}] style={p.style.name} align={p.alignment} runs={runs}")


if __name__ == "__main__":
    main()