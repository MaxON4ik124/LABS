from io import BytesIO
from copy import deepcopy

from docx import Document
from docx.text.paragraph import Paragraph
from docx.table import Table, _Cell
from docx.oxml.ns import qn
from docx.shared import Pt

from PIL import Image

NS = {
    "w": "http://schemas.openxmlformats.org/wordprocessingml/2006/main",
    "a": "http://schemas.openxmlformats.org/drawingml/2006/main",
    "r": "http://schemas.openxmlformats.org/officeDocument/2006/relationships",
    "v": "urn:schemas-microsoft-com:vml",
}

def iter_block_items(parent):
    """Paragraph и Table в порядке следования (Document или Cell)."""
    if isinstance(parent, _Cell):
        parent_elm = parent._tc
    else:
        parent_elm = parent.element.body

    for child in parent_elm.iterchildren():
        if child.tag == qn("w:p"):
            yield Paragraph(child, parent)
        elif child.tag == qn("w:tbl"):
            yield Table(child, parent)

def iter_paragraph_content(paragraph, doc_part):
    """
    Контент параграфа в порядке следования:
    - text (w:t, w:tab, w:br)
    - images (w:drawing -> a:blip@r:embed, w:pict -> v:imagedata@r:id)
    """
    for run in paragraph.runs:
        r = run._element
        for node in r.iterchildren():
            # --- TEXT ---
            if node.tag == qn("w:t"):
                txt = node.text or ""
                if txt:
                    yield {"type": "text", "text": txt}
            elif node.tag == qn("w:tab"):
                yield {"type": "text", "text": "\t"}
            elif node.tag == qn("w:br"):
                yield {"type": "text", "text": "\n"}

            # --- IMAGES (DrawingML) ---
            elif node.tag == qn("w:drawing"):
                rIds = node.xpath(".//a:blip/@r:embed", namespaces=NS)
                for rId in rIds:
                    part = doc_part.related_parts[rId]
                    yield {"type": "image", "blob": part.blob, "partname": str(part.partname)}

            # --- IMAGES (VML legacy) ---
            elif node.tag == qn("w:pict"):
                rIds = node.xpath(".//v:imagedata/@r:id", namespaces=NS)
                for rId in rIds:
                    part = doc_part.related_parts[rId]
                    yield {"type": "image", "blob": part.blob, "partname": str(part.partname)}

def iter_docx_content(doc):
    """Единый генератор по документу (включая таблицы)."""
    for block in iter_block_items(doc):
        if isinstance(block, Paragraph):
            yield from iter_paragraph_content(block, doc.part)
        elif isinstance(block, Table):
            for row in block.rows:
                for cell in row.cells:
                    for inner in iter_block_items(cell):
                        if isinstance(inner, Paragraph):
                            yield from iter_paragraph_content(inner, doc.part)
                        elif isinstance(inner, Table):
                            # вложенные таблицы (на практике редко, но пусть будет)
                            for r2 in inner.rows:
                                for c2 in r2.cells:
                                    for inner2 in iter_block_items(c2):
                                        if isinstance(inner2, Paragraph):
                                            yield from iter_paragraph_content(inner2, doc.part)

def resize_image_blob_50(blob: bytes, fallback_format: str = "PNG") -> BytesIO:
    """
    Уменьшает картинку по пикселям на 50% (w,h -> w/2,h/2), сохраняет в BytesIO.
    Формат стараемся сохранить исходный (если Pillow его понимает).
    """
    img = Image.open(BytesIO(blob))
    w, h = img.size
    new_size = (max(1, w // 2), max(1, h // 2))
    img2 = img.resize(new_size, Image.LANCZOS)

    out = BytesIO()
    fmt = (img.format or fallback_format).upper()

    # На всякий случай нормализуем форматы для Pillow
    if fmt == "JPG":
        fmt = "JPEG"

    # Если у JPEG есть альфа (редко, но бывает после конвертаций) — уберём
    if fmt in ("JPEG", "JPG") and img2.mode in ("RGBA", "LA"):
        img2 = img2.convert("RGB")

    img2.save(out, format=fmt)
    out.seek(0)
    return out

def append_text_run(p, text: str, size_pt: int = 6):
    """
    Добавляет текст в текущий параграф, шрифт 6pt.
    Переводы строки '\n' разбиваем на реальные переносы в Word.
    """
    parts = text.split("\n")
    for i, part in enumerate(parts):
        if part:
            r = p.add_run(part)
            r.font.size = Pt(size_pt)
        if i != len(parts) - 1:
            r = p.add_run()
            r.add_break()  # перенос строки
            r.font.size = Pt(size_pt)

def parse_and_insert(file1: str, file2: str):
    doc1 = Document(file1)
    doc2 = Document()

    # Будем писать в конец doc2, в “текущий” параграф
    current_p = doc2.add_paragraph()

    for item in iter_docx_content(doc1):
        if item["type"] == "text":
            append_text_run(current_p, item["text"], size_pt=6)
        elif item["type"] == "image":
            # Перед картинкой лучше завершить текстовый параграф и вставить картинку в новый
            current_p = doc2.add_paragraph()
            img_stream = resize_image_blob_50(item["blob"])
            current_p.add_run().add_picture(img_stream)
            # После картинки продолжим с нового параграфа
            current_p = doc2.add_paragraph()

    doc2.save(file2)

if __name__ == "__main__":
    parse_and_insert("bilety.docx", "A.docx",)
    print("Готово: file2_out.docx")
