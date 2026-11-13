# -*- coding: utf-8 -*-
"""Простая презентация на 8 слайдов: Вариант 7 — антивирусное средство.

Тема: организация без гостайны, финансовая сфера, большая нагрузка.
Предлагаемое решение: Kaspersky Endpoint Security (KES).
Генерация: python-pptx, минимальный чистый стиль (16:9).
"""
import os

from pptx import Presentation
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN
from pptx.util import Inches, Pt

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(BASE, "Five", "Law", "Антивирус_вариант_7_KES.pptx")

# Палитра
DARK = RGBColor(0x18, 0x2A, 0x41)     # тёмно-синий (заголовки)
TEXT = RGBColor(0x26, 0x37, 0x4A)     # основной текст
GREY = RGBColor(0x68, 0x79, 0x8C)     # подписи
ACCENT = RGBColor(0x13, 0x8B, 0x8A)   # бирюзовый акцент
FONT = "Aptos"


def add_text(slide, text, left, top, width, height,
             size=16, bold=False, color=TEXT, align=PP_ALIGN.LEFT, line_spacing=1.0):
    """Добавляет текстовое поле. Поддерживает многострочный текст."""
    box = slide.shapes.add_textbox(Inches(left), Inches(top), Inches(width), Inches(height))
    tf = box.text_frame
    tf.word_wrap = True
    lines = text.split("\n")
    for i, line in enumerate(lines):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.alignment = align
        p.line_spacing = line_spacing
        run = p.add_run()
        run.text = line
        run.font.name = FONT
        run.font.size = Pt(size)
        run.font.bold = bold
        run.font.color.rgb = color
    return box


def add_title(slide, title, subtitle=None):
    """Заголовок слайда и необязательный подзаголовок."""
    add_text(slide, title, 0.7, 0.45, 11.9, 0.8, size=30, bold=True, color=DARK)
    if subtitle:
        add_text(slide, subtitle, 0.72, 1.28, 11.9, 0.5, size=15, color=GREY)


def add_card(slide, num, heading, body, top, left=0.75, width=11.8, height=1.25, accent=ACCENT):
    """Карточка-строка: номер, заголовок, описание."""
    box = slide.shapes.add_shape(1, Inches(left), Inches(top), Inches(width), Inches(height))
    box.fill.solid()
    box.fill.fore_color.rgb = RGBColor(0xFF, 0xFF, 0xFF)
    box.line.color.rgb = RGBColor(0xE2, 0xE9, 0xF0)
    bar = slide.shapes.add_shape(1, Inches(left), Inches(top), Inches(0.09), Inches(height))
    bar.fill.solid()
    bar.fill.fore_color.rgb = accent
    bar.line.fill.background()
    if num:
        add_text(slide, num, left + 0.2, top + 0.18, 0.6, 0.3, size=16, bold=True, color=accent)
    hx = left + 0.95 if num else left + 0.3
    add_text(slide, heading, hx, top + 0.15, width - 1.2, 0.35, size=15, bold=True, color=DARK)
    add_text(slide, body, hx, top + 0.55, width - 1.2, height - 0.6, size=12, color=TEXT)


def add_bullets(slide, items, left, top, width, height, size=14, color=TEXT):
    """Маркированный список."""
    box = slide.shapes.add_textbox(Inches(left), Inches(top), Inches(width), Inches(height))
    tf = box.text_frame
    tf.word_wrap = True
    for i, item in enumerate(items):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.line_spacing = 1.15
        run = p.add_run()
        run.text = "•  " + item
        run.font.name = FONT
        run.font.size = Pt(size)
        run.font.color.rgb = color
    return box


def add_table(slide, data, left, top, width, height, col_widths=None, font_size=11):
    """Таблица с шапкой."""
    rows, cols = len(data), len(data[0])
    shape = slide.shapes.add_table(rows, cols, Inches(left), Inches(top),
                                   Inches(width), Inches(height))
    table = shape.table
    if col_widths:
        for i, w in enumerate(col_widths):
            table.columns[i].width = Inches(w)
    for r, row in enumerate(data):
        for c, value in enumerate(row):
            cell = table.cell(r, c)
            cell.text = str(value)
            para = cell.text_frame.paragraphs[0]
            for run in para.runs:
                run.font.name = FONT
                run.font.size = Pt(font_size)
                run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF) if r == 0 else TEXT
                run.font.bold = (r == 0)
    return table


def blank(prs):
    return prs.slides.add_slide(prs.slide_layouts[6])


def slide_1_title(prs):
    s = blank(prs)
    add_text(s, "ПРАКТИЧЕСКАЯ РАБОТА  ·  ВАРИАНТ 7", 0.82, 0.62, 9.0, 0.3,
             size=12, bold=True, color=ACCENT)
    add_text(s, "Государственные реестры\nМинцифры / Роскомнадзора / ФСТЭК",
             0.82, 1.5, 11.5, 1.7, size=38, bold=True, color=DARK)
    add_text(s, "Подбор средства защиты для организации:\nантивирусное средство, без гостайны, "
                "финансовая сфера, большая нагрузка",
             0.85, 3.35, 11.4, 1.0, size=17, color=GREY, line_spacing=1.2)
    add_card(s, "", "Предлагаемое решение",
             "Kaspersky Endpoint Security (KES) — Лаборатория Касперского",
             4.7, height=0.95)
    add_text(s, "Выполнил: ____________________      Группа: ____________",
             0.85, 6.3, 11.8, 0.4, size=13, color=GREY)


def slide_2_task(prs):
    s = blank(prs)
    add_title(s, "Постановка задачи",
              "Исходные условия варианта 7 и что нужно подобрать")
    add_card(s, "01", "Особенности организации",
             "Нет сведений, содержащих государственную тайну; финансовая сфера; большая нагрузка.",
             2.1)
    add_card(s, "02", "Что подобрать из реестров",
             "Решение из реестра российского ПО + сертифицированное СЗИ ФСТЭК с учётом срока "
             "действия сертификата.", 3.55)
    add_bullets(s, [
        "Лицензиат на разработку/производство СЗИ (реестр лицензий).",
        "Орган по аттестации объектов информатизации в регионе.",
        "Аккредитованная испытательная лаборатория ФСТЭК в регионе.",
        "Обосновать выбор и оформить презентацию 5–8 минут.",
    ], 0.75, 5.1, 11.8, 1.8, size=13.5)


def slide_3_requirements(prs):
    s = blank(prs)
    add_title(s, "Особенности применения и требования",
              "Что означает «финансовая сфера + большая нагрузка + без гостайны»")
    add_card(s, "01", "Без государственной тайны",
             "Гостайна отсутствует → не нужен класс защищённости по гостайне; "
             "достаточно сертификации ФСТЭК для ИСПДн/ГИС.", 2.1)
    add_card(s, "02", "Финансовая сфера",
             "Требования ЦБ РФ (683-П/719-П), ГОСТ Р 57580; антивирусная защита обязательна. "
             "Ключ — 152-ФЗ и защита ПДн клиентов.", 3.55, accent=ACCENT)
    add_card(s, "03", "Большая нагрузка",
             "Нужна производительность, централизованное управление, тонкие политики и "
             "исключения для транзакционных систем и СУБД.", 5.0, accent=ACCENT)


def slide_4_registries(prs):
    s = blank(prs)
    add_title(s, "Проверка по государственным реестрам",
              "Реестр российского ПО и реестр сертифицированных СЗИ ФСТЭК")
    data = [
        ["Реестр", "Что проверяем", "Результат"],
        ["Реестр российского ПО\n(Минцифры)", "Наличие KES в записи реестра",
         "Есть позиция «Лаборатория Касперского»\n(класс — средства защиты информации)"],
        ["Реестр сертифицированных\nСЗИ ФСТЭК", "Сертификат на редакцию KES",
         "Есть сертифицированная сборка KES\n(уровень доверия, защита от НДВ)"],
        ["Срок действия сертификата", "Проверить актуальность",
         "Указать номер и срок действия\nсертификата на дату закупки"],
    ]
    add_table(s, data, 0.75, 2.1, 11.8, 3.4, col_widths=[3.3, 3.6, 4.9], font_size=11.5)
    add_text(s, "Примечание: выписки из реестров и номер сертификата приложить скриншотами.",
             0.75, 5.75, 11.8, 0.4, size=12, color=GREY)


def slide_5_compare(prs):
    s = blank(prs)
    add_title(s, "Сравнение возможных вариантов",
              "Антивирусные средства: наличие в реестрах и применимость")
    data = [
        ["Решение", "Реестр рос. ПО", "Сертификат ФСТЭК", "Вывод"],
        ["Kaspersky Endpoint Security", "Да", "Да (сертиф. сборка)", "Выбран"],
        ["Dr.Web (ES for Business)", "Да", "Да", "Альтернатива"],
        ["Средство защиты «Соболь» и др. VipNet", "Да", "Да", "Не антивирус"],
        ["Зарубежные AV (без реестра)", "Нет", "Нет", "Не подходит"],
    ]
    add_table(s, data, 0.75, 2.1, 11.8, 2.9, col_widths=[4.3, 2.4, 2.9, 2.2], font_size=11.5)
    add_card(s, "", "Критерии выбора",
             "Наличие в реестре российского ПО + действующий сертификат ФСТЭК + "
             "производительность при большой нагрузке.", 5.25, height=0.95)


def slide_6_choice(prs):
    s = blank(prs)
    add_title(s, "Обоснование выбора: Kaspersky Endpoint Security",
              "Почему решение подходит организации финансовой сферы")
    add_card(s, "01", "Соответствие реестрам",
             "KES есть в реестре российского ПО, а сертифицированная сборка — в реестре СЗИ ФСТЭК.",
             2.1)
    add_card(s, "02", "Регуляторные требования",
             "Закрывает требования 152-ФЗ, ЦБ РФ (683-П/719-П) и ГОСТ Р 57580 для финансовой сферы.",
             3.55, accent=ACCENT)
    add_card(s, "03", "Большая нагрузка",
             "Масштабируется через Kaspersky Security Center; тонкие политики и исключения для "
             "транзакционных систем.", 5.0, accent=ACCENT)


def slide_7_infra(prs):
    s = blank(prs)
    add_title(s, "Состав решения и внедрение",
              "Компоненты KES для организации с большой нагрузкой")
    add_card(s, "01", "Kaspersky Endpoint Security",
             "Защита рабочих станций и серверов: файлы, веб, почта, поведенческий анализ.", 2.1)
    add_card(s, "02", "Kaspersky Security Center",
             "Централизованное управление, политики, отчёты, масштабирование на тысячи узлов.",
             3.45, accent=ACCENT)
    add_card(s, "03", "Серверные и виртуальные среды",
             "KES for Linux/серверов, Kaspersky Security for Virtualization — снижение нагрузки "
             "на инфраструктуру.", 4.8, accent=ACCENT)
    add_bullets(s, [
        "Исключения для СУБД и транзакционных систем.",
        "Пилот на одном сегменте, затем поэтапное развёртывание.",
    ], 0.75, 6.15, 11.8, 0.9, size=13)


def slide_8_sources(prs):
    s = blank(prs)
    add_title(s, "Источники и выводы")
    add_bullets(s, [
        "Реестр российского ПО — reestr.digital.gov.ru",
        "Реестр сертифицированных СЗИ ФСТЭК — fstec.ru (раздел «Сертификация»)",
        "Реестр лицензиатов ФСТЭК на разработку/производство СЗИ",
        "Перечни органов по аттестации и аккредитованных лабораторий ФСТЭК",
        "Документация Kaspersky Endpoint Security и Kaspersky Security Center",
    ], 0.75, 2.1, 11.8, 2.6, size=14)
    add_card(s, "", "Вывод",
             "Kaspersky Endpoint Security — обоснованный выбор: есть в реестре российского ПО, "
             "сертифицирован ФСТЭК и подходит для финансовой сферы при большой нагрузке.",
             5.0, height=1.25)


def main():
    prs = Presentation()
    prs.slide_width = Inches(13.333)
    prs.slide_height = Inches(7.5)
    slide_1_title(prs)
    slide_2_task(prs)
    slide_3_requirements(prs)
    slide_4_registries(prs)
    slide_5_compare(prs)
    slide_6_choice(prs)
    slide_7_infra(prs)
    slide_8_sources(prs)
    prs.save(OUT)
    print("Слайдов:", len(prs.slides.__iter__.__self__._sldIdLst))
    print("Сохранено:", OUT)


if __name__ == "__main__":
    main()




