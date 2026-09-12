import telebot
from telebot import types
import matplotlib
matplotlib.use('Agg')  # Используем Agg backend для избежания проблем с GUI
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from io import BytesIO
import math
import threading
import gc

# Замени на свой токен бота
BOT_TOKEN = "7203298205:AAF3saTT5Z4wqRsh6zdokbNd1nGA0EMxIS0"
bot = telebot.TeleBot(BOT_TOKEN, num_threads=28)
Whitelist = [966064199, 837605460, 1013541709, 1480791192]
# Хранилище состояний пользователей
user_states = {}
class TreeNode:
    """Узел для AVL / RB / Splay"""
    def __init__(self, val=0, color='red'):
        self.val = val
        self.left = None
        self.right = None
        self.height = 1  # для AVL
        self.color = color  # для RB
        self.parent = None  # для RB


class DBTNode:
    """Узел для B‑/DB‑/SDB‑дерева (t = 1)."""
    def __init__(self, is_leaf=True):
        self.keys: list[int] = []
        self.children: list['DBTNode'] = []
        self.is_leaf = is_leaf

class DBTree:
    MAX_KEYS = 2  # при 3 — переполнение

    def __init__(self):
        self.root = DBTNode(is_leaf=True)

    # -------- ВСТАВКА --------
    def insert(self, key: int):
        split = self._insert_rec(self.root, key)
        if split:
            mid, right = split
            new_root = DBTNode(is_leaf=False)
            new_root.keys = [mid]
            new_root.children = [self.root, right]
            self.root = new_root

    def _insert_rec(self, node: DBTNode, key: int):
        if node.is_leaf:
            i = len(node.keys) - 1
            node.keys.append(0)
            while i >= 0 and key < node.keys[i]:
                node.keys[i + 1] = node.keys[i]
                i -= 1
            node.keys[i + 1] = key
            return None if len(node.keys) <= self.MAX_KEYS else self._split(node)

        idx = 0
        while idx < len(node.keys) and key > node.keys[idx]:
            idx += 1
        split = self._insert_rec(node.children[idx], key)
        if split:
            mid, right = split
            node.keys.insert(idx, mid)
            node.children.insert(idx + 1, right)
        return None if len(node.keys) <= self.MAX_KEYS else self._split(node)

    def _split(self, node: DBTNode):
        mid_key = node.keys[1]
        right = DBTNode(is_leaf=node.is_leaf)
        right.keys = node.keys[2:]
        node.keys = node.keys[:1]
        if not node.is_leaf:
            right.children = node.children[2:]
            node.children = node.children[:2]
        return mid_key, right

# ──────────────────────────────────────────────────────────────
# 2️⃣  СДБ‑ДЕРЕВО  (t = 1, левый и правый брат)  ⇢  ПОЛНЫЙ delete
# ──────────────────────────────────────────────────────────────
class SDBTree(DBTree):
    """Симметричное ДБ (можно заимствовать слева и справа при удалении)."""

    # ---- PUBLIC API ----
    def delete(self, key: int):
        self._delete_rec(self.root, key)
        if not self.root.is_leaf and len(self.root.keys) == 0:
            self.root = self.root.children[0]

    # ---- INTERNALS ----
    def _delete_rec(self, node: DBTNode, key: int):
        # поиск позиции ключа в node
        i = 0
        while i < len(node.keys) and key > node.keys[i]:
            i += 1

        # ▸ Ключ найден в node
        if i < len(node.keys) and node.keys[i] == key:
            if node.is_leaf:
                node.keys.pop(i)
                return
            # внутренний узел → заменить предшественником / преемником / слить
            left, right = node.children[i], node.children[i + 1]
            if len(left.keys) > 1:
                node.keys[i] = self._pop_max(left)
            elif len(right.keys) > 1:
                node.keys[i] = self._pop_min(right)
            else:
                self._merge(node, i)
                self._delete_rec(left, key)
            return

        # ▸ Ключ НЕ найден в node
        if node.is_leaf:
            return  # нет такого ключа

        # перед спуском гарантируем, что у ребёнка будет ≥ 2 ключей
        self._fix_child(node, i)
        self._delete_rec(node.children[i], key)

    # ---- фиксация недоузла ----
    def _fix_child(self, parent: DBTNode, idx: int):
        child = parent.children[idx]
        if len(child.keys) > 1:
            return
        # пробуем левый брат
        if idx > 0 and len(parent.children[idx - 1].keys) > 1:
            left = parent.children[idx - 1]
            borrow = left.keys.pop()
            child.keys.insert(0, parent.keys[idx - 1])
            parent.keys[idx - 1] = borrow
            if not left.is_leaf:
                child.children.insert(0, left.children.pop())
            return
        # пробуем правый брат
        if idx < len(parent.children) - 1 and len(parent.children[idx + 1].keys) > 1:
            right = parent.children[idx + 1]
            borrow = right.keys.pop(0)
            child.keys.append(parent.keys[idx])
            parent.keys[idx] = borrow
            if not right.is_leaf:
                child.children.append(right.children.pop(0))
            return
        # иначе — слияние
        if idx > 0:
            self._merge(parent, idx - 1)
        else:
            self._merge(parent, idx)

    def _merge(self, parent: DBTNode, idx: int):
        left = parent.children[idx]
        right = parent.children[idx + 1]
        left.keys.append(parent.keys.pop(idx))
        left.keys.extend(right.keys)
        if not left.is_leaf:
            left.children.extend(right.children)
        parent.children.pop(idx + 1)

    # ---- утилиты min / max ----
    def _pop_min(self, node: DBTNode) -> int:
        cur = node
        while not cur.is_leaf:
            self._fix_child(cur, 0)
            cur = cur.children[0]
        return cur.keys.pop(0)

    def _pop_max(self, node: DBTNode) -> int:
        cur = node
        while not cur.is_leaf:
            last = len(cur.children) - 1
            self._fix_child(cur, last)
            cur = cur.children[last]
        return cur.keys.pop()

# ──────────────────────────────────────────────────────────────
# 3️⃣  B‑ДЕРЕВО (t ≥ 2)
# ──────────────────────────────────────────────────────────────
class BTreeNode:
    def __init__(self, is_leaf=False):
        self.keys = []
        self.children = []
        self.is_leaf = is_leaf

class BTree:
    def __init__(self, degree: int):
        self.root = BTreeNode(is_leaf=True)
        self.t = degree

    def insert(self, key):
        root = self.root
        if len(root.keys) == 2 * self.t - 1:
            new_root = BTreeNode(is_leaf=False)
            new_root.children.append(root)
            self._split_child(new_root, 0)
            self.root = new_root
        self._insert_non_full(self.root, key)

    def _split_child(self, parent, idx):
        t = self.t
        full = parent.children[idx]
        right = BTreeNode(is_leaf=full.is_leaf)
        mid = full.keys[t - 1]
        right.keys = full.keys[t:]
        full.keys = full.keys[:t - 1]
        if not full.is_leaf:
            right.children = full.children[t:]
            full.children = full.children[:t]
        parent.children.insert(idx + 1, right)
        parent.keys.insert(idx, mid)

    def _insert_non_full(self, node, key):
        i = len(node.keys) - 1
        if node.is_leaf:
            node.keys.append(0)
            while i >= 0 and key < node.keys[i]:
                node.keys[i + 1] = node.keys[i]
                i -= 1
            node.keys[i + 1] = key
        else:
            while i >= 0 and key < node.keys[i]:
                i -= 1
            i += 1
            if len(node.children[i].keys) == 2 * self.t - 1:
                self._split_child(node, i)
                if key > node.keys[i]:
                    i += 1
            self._insert_non_full(node.children[i], key)



class RedBlackTree:
    def __init__(self):
        self.NIL = TreeNode(0, 'black')
        self.root = self.NIL

    def left_rotate(self, x):
        y = x.right
        x.right = y.left
        if y.left != self.NIL:
            y.left.parent = x
        y.parent = x.parent
        if x.parent == self.NIL:
            self.root = y
        elif x == x.parent.left:
            x.parent.left = y
        else:
            x.parent.right = y
        y.left = x
        x.parent = y

    def right_rotate(self, x):
        y = x.left
        x.left = y.right
        if y.right != self.NIL:
            y.right.parent = x
        y.parent = x.parent
        if x.parent == self.NIL:
            self.root = y
        elif x == x.parent.right:
            x.parent.right = y
        else:
            x.parent.left = y
        y.right = x
        x.parent = y

    def insert_fixup(self, z):
        while z.parent.color == 'red':
            if z.parent == z.parent.parent.left:
                y = z.parent.parent.right
                if y.color == 'red':
                    z.parent.color = 'black'
                    y.color = 'black'
                    z.parent.parent.color = 'red'
                    z = z.parent.parent
                else:
                    if z == z.parent.right:
                        z = z.parent
                        self.left_rotate(z)
                    z.parent.color = 'black'
                    z.parent.parent.color = 'red'
                    self.right_rotate(z.parent.parent)
            else:
                y = z.parent.parent.left
                if y.color == 'red':
                    z.parent.color = 'black'
                    y.color = 'black'
                    z.parent.parent.color = 'red'
                    z = z.parent.parent
                else:
                    if z == z.parent.left:
                        z = z.parent
                        self.right_rotate(z)
                    z.parent.color = 'black'
                    z.parent.parent.color = 'red'
                    self.left_rotate(z.parent.parent)
        self.root.color = 'black'

    def insert(self, key):
        z = TreeNode(key, 'red')
        z.left = self.NIL
        z.right = self.NIL
        y = self.NIL
        x = self.root

        while x != self.NIL:
            y = x
            if z.val < x.val:
                x = x.left
            else:
                x = x.right

        z.parent = y
        if y == self.NIL:
            self.root = z
        elif z.val < y.val:
            y.left = z
        else:
            y.right = z

        if z.parent == self.NIL:
            z.color = 'black'
            return

        if z.parent.parent == self.NIL:
            return

        self.insert_fixup(z)

class AVLTree:
    def get_height(self, root):
        if not root:
            return 0
        return root.height

    def get_balance(self, root):
        if not root:
            return 0
        return self.get_height(root.left) - self.get_height(root.right)

    def rotate_right(self, y):
        x = y.left
        T2 = x.right
        x.right = y
        y.left = T2
        y.height = 1 + max(self.get_height(y.left), self.get_height(y.right))
        x.height = 1 + max(self.get_height(x.left), self.get_height(x.right))
        return x

    def rotate_left(self, x):
        y = x.right
        T2 = y.left
        y.left = x
        x.right = T2
        x.height = 1 + max(self.get_height(x.left), self.get_height(x.right))
        y.height = 1 + max(self.get_height(y.left), self.get_height(y.right))
        return y

    def insert(self, root, key):
        if not root:
            return TreeNode(key)
        
        if key < root.val:
            root.left = self.insert(root.left, key)
        elif key > root.val:
            root.right = self.insert(root.right, key)
        else:
            return root

        root.height = 1 + max(self.get_height(root.left), self.get_height(root.right))
        balance = self.get_balance(root)

        if balance > 1 and key < root.left.val:
            return self.rotate_right(root)
        if balance < -1 and key > root.right.val:
            return self.rotate_left(root)
        if balance > 1 and key > root.left.val:
            root.left = self.rotate_left(root.left)
            return self.rotate_right(root)
        if balance < -1 and key < root.right.val:
            root.right = self.rotate_right(root.right)
            return self.rotate_left(root)
        return root

class SplayTree:
    def right_rotate(self, x):
        y = x.left
        x.left = y.right
        y.right = x
        return y

    def left_rotate(self, x):
        y = x.right
        x.right = y.left
        y.left = x
        return y

    def splay(self, root, key):
        if not root or root.val == key:
            return root

        if root.val > key:
            if not root.left:
                return root
            
            if root.left.val > key:
                root.left.left = self.splay(root.left.left, key)
                root = self.right_rotate(root)
            elif root.left.val < key:
                root.left.right = self.splay(root.left.right, key)
                if root.left.right:
                    root.left = self.left_rotate(root.left)
            
            return self.right_rotate(root) if root.left else root
        else:
            if not root.right:
                return root
            
            if root.right.val > key:
                root.right.left = self.splay(root.right.left, key)
                if root.right.left:
                    root.right = self.right_rotate(root.right)
            elif root.right.val < key:
                root.right.right = self.splay(root.right.right, key)
                root = self.left_rotate(root)
            
            return self.left_rotate(root) if root.right else root

    def insert(self, root, key):
        if not root:
            return TreeNode(key)
        
        root = self.splay(root, key)
        
        if root.val == key:
            return root
        
        new_node = TreeNode(key)
        
        if root.val > key:
            new_node.right = root
            new_node.left = root.left
            root.left = None
        else:
            new_node.left = root
            new_node.right = root.right
            root.right = None
        
        return new_node
def draw_tree(root=None, tree_type="Binary", btree=None):
    plt.ioff()
    fig, ax = plt.subplots(figsize=(14, 10))  # Увеличиваем размер для лучшего отображения
    ax.set_xlim(0, 12)
    ax.set_ylim(0, 10)
    ax.axis('off')
    try:
        if tree_type in ("B", "DB", "SDB") and btree:
            draw_btree(ax, btree.root, 6, 8, 5, tree_type)
        elif root:
            draw_binary_tree(ax, root, 6, 8, 2, tree_type)
        plt.title(f"{tree_type}-дерево", fontsize=16)
        buf = BytesIO()
        plt.savefig(buf, format='png', bbox_inches='tight', dpi=300)
        buf.seek(0)
        return buf
    finally:
        plt.close(fig)
        plt.clf()
        gc.collect()

def draw_binary_tree(ax, node, x, y, dx, tree_type, visited=None):
    if not node or (hasattr(node, 'val') and node.val == 0 and getattr(node, 'color', 'black') == 'black'):
        return
    if visited is None:
        visited = set()
    if id(node) in visited:
        return
    visited.add(id(node))

    fill, fg = ('red', 'white') if tree_type == 'Red-Black' and node.color == 'red' else (
        ('black', 'white') if tree_type == 'Red-Black' else ('lightblue', 'black'))
    ax.add_patch(plt.Circle((x, y), 0.3, color=fill, ec='black', linewidth=2))
    ax.text(x, y, str(node.val), ha='center', va='center', color=fg, fontsize=12, fontweight='bold')

    if node.left:
        ax.plot([x, x - dx], [y, y - 1], 'k-', linewidth=3)
        draw_binary_tree(ax, node.left, x - dx, y - 1, dx * 0.7, tree_type, visited)
    if node.right:
        ax.plot([x, x + dx], [y, y - 1], 'k-', linewidth=3)
        draw_binary_tree(ax, node.right, x + dx, y - 1, dx * 0.7, tree_type, visited)

def draw_btree(ax, node, x, y, width, tree_kind="B"):
    """
    Отрисовка B-дерева с отдельными узлами для каждого ключа,
    соединёнными горизонтальными пунктирными линиями (братские связи)
    """
    if not node:
        return

    # Рисуем каждый ключ как отдельный узел
    key_positions = _draw_node_keys_separately(ax, node, x, y)
    
    # Если узел не листовой, рисуем детей
    if not node.is_leaf:
        _draw_children_with_connections(ax, node, key_positions, y, width, tree_kind)

def _draw_node_keys_separately(ax, node, center_x, y):
    """
    Рисует каждый ключ узла как отдельный круг,
    соединённый горизонтальными пунктирными линиями с соседними ключами
    """
    keys = node.keys
    if not keys:
        return []
    
    # Расстояние между ключами
    key_spacing = 0.8
    total_width = (len(keys) - 1) * key_spacing
    start_x = center_x - total_width / 2
    
    key_positions = []
    
    # Рисуем каждый ключ как отдельный круг
    for i, key in enumerate(keys):
        key_x = start_x + i * key_spacing
        key_positions.append((key_x, y, key))
        
        # Рисуем круг для ключа
        color = 'lightgreen' if node.is_leaf else 'lightblue'
        ax.add_patch(plt.Circle((key_x, y), 0.25, 
                               color=color, ec='black', linewidth=2))
        ax.text(key_x, y, str(key), 
               ha='center', va='center', 
               fontsize=10, fontweight='bold')
    
    # Соединяем соседние ключи горизонтальными пунктирными линиями
    for i in range(len(key_positions) - 1):
        x1, y1, _ = key_positions[i]
        x2, y2, _ = key_positions[i + 1]
        
        # Горизонтальная пунктирная линия между братьями
        ax.plot([x1 + 0.25, x2 - 0.25], [y1, y2], 
               linestyle='--', color='red', linewidth=2, alpha=0.8)
    
    return key_positions

def _draw_children_with_connections(ax, node, key_positions, parent_y, width, tree_kind):
    """
    Рисует детей узла и соединяет их с соответствующими ключами родителя
    """
    if len(node.children) == 0:
        return
    
    child_y = parent_y - 2.0  # Расстояние между уровнями
    child_spacing = width / len(node.children)
    
    # Определяем позиции детей
    children_positions = []
    for i, child in enumerate(node.children):
        child_x = key_positions[0][0] - width/2 + child_spacing/2 + i * child_spacing
        children_positions.append((child_x, child_y, child))
    
    # Рисуем вертикальные связи от ключей к детям
    for i, (child_x, child_y, child) in enumerate(children_positions):
        # Определяем, к какому ключу должна идти связь
        if i < len(key_positions):
            # Связь от ключа к левому ребёнку
            key_x, key_y, _ = key_positions[i] if i < len(key_positions) else key_positions[-1]
        else:
            # Для последнего ребёнка - от последнего ключа
            key_x, key_y, _ = key_positions[-1]
        
        # Если это не первый ребёнок, соединяем с предыдущим ключом
        if i > 0 and i <= len(key_positions):
            key_x, key_y, _ = key_positions[i-1]
        
        # Вертикальная связь
        ax.plot([key_x, child_x], [key_y - 0.25, child_y + 0.3], 
               'k-', linewidth=2)
        
        # Рекурсивно рисуем ребёнка
        draw_btree(ax, child, child_x, child_y, child_spacing * 0.8, tree_kind)

def draw_btree_alternative(ax, node, x, y, width, tree_kind="B"):
    """
    Альтернативная версия: показывает структуру B-дерева с явными братскими связями
    """
    if not node:
        return

    keys = node.keys
    if len(keys) == 1:
        # Одиночный ключ - рисуем как обычный узел
        _draw_single_key_node(ax, keys[0], x, y, node.is_leaf)
        key_positions = [(x, y, keys[0])]
    else:
        # Множественные ключи - рисуем как связанную цепочку
        key_positions = _draw_linked_keys(ax, keys, x, y, node.is_leaf)
    
    # Рисуем детей, если есть
    if not node.is_leaf and node.children:
        _draw_children_advanced(ax, node, key_positions, y, width, tree_kind)

def _draw_single_key_node(ax, key, x, y, is_leaf):
    """Рисует один ключ как круг"""
    color = 'lightgreen' if is_leaf else 'lightblue'
    ax.add_patch(plt.Circle((x, y), 0.3, 
                           color=color, ec='black', linewidth=2))
    ax.text(x, y, str(key), 
           ha='center', va='center', 
           fontsize=12, fontweight='bold')

def _draw_linked_keys(ax, keys, center_x, y, is_leaf):
    """Рисует ключи как связанную цепочку узлов"""
    key_spacing = 0.7
    total_width = (len(keys) - 1) * key_spacing
    start_x = center_x - total_width / 2
    
    key_positions = []
    color = 'lightgreen' if is_leaf else 'lightblue'
    
    for i, key in enumerate(keys):
        key_x = start_x + i * key_spacing
        key_positions.append((key_x, y, key))
        
        # Рисуем узел
        ax.add_patch(plt.Circle((key_x, y), 0.25, 
                               color=color, ec='black', linewidth=2))
        ax.text(key_x, y, str(key), 
               ha='center', va='center', 
               fontsize=10, fontweight='bold')
        
        # Горизонтальная связь с следующим ключом
        if i < len(keys) - 1:
            next_x = start_x + (i + 1) * key_spacing
            ax.plot([key_x + 0.25, next_x - 0.25], [y, y], 
                   linestyle='--', color='red', linewidth=2)
    
    return key_positions

def _draw_children_advanced(ax, node, key_positions, parent_y, width, tree_kind):
    """Продвинутая отрисовка детей с правильными связями"""
    child_y = parent_y - 2.0
    num_children = len(node.children)
    
    # Вычисляем позиции детей
    if len(key_positions) > 1:
        # Для множественных ключей - равномерно распределяем детей
        left_x = key_positions[0][0]
        right_x = key_positions[-1][0]
        child_width = (right_x - left_x) * 1.5
        child_spacing = child_width / max(1, num_children - 1) if num_children > 1 else 0
        start_x = left_x - child_width * 0.25
    else:
        # Для одного ключа
        center_x = key_positions[0][0]
        child_spacing = width / num_children
        start_x = center_x - width/2 + child_spacing/2
        child_spacing = width / num_children
    
    # Рисуем детей и связи
    for i, child in enumerate(node.children):
        if num_children == 1:
            child_x = key_positions[0][0]
        else:
            child_x = start_x + i * child_spacing
        
        # Определяем родительский ключ для связи
        parent_key_idx = min(i, len(key_positions) - 1)
        parent_x, parent_y, _ = key_positions[parent_key_idx]
        
        # Вертикальная связь
        ax.plot([parent_x, child_x], [parent_y - 0.3, child_y + 0.3], 
               'k-', linewidth=2)
        
        # Рекурсивно рисуем ребёнка
        draw_btree_alternative(ax, child, child_x, child_y, 
                             child_spacing * 0.8, tree_kind)
        


@bot.message_handler(commands=['start'], func=lambda m: m.from_user.id in Whitelist)
def cmd_start(m):
    kb = types.InlineKeyboardMarkup(row_width=2)
    kb.add(
        types.InlineKeyboardButton('AVL‑дерево', callback_data='AVL'),
        types.InlineKeyboardButton('Красно‑чёрное дерево', callback_data='Red-Black'),
        types.InlineKeyboardButton('Splay‑дерево', callback_data='Splay'),
        types.InlineKeyboardButton('B‑дерево', callback_data='B'),
        types.InlineKeyboardButton('ДБ‑дерево', callback_data='DB'),
        types.InlineKeyboardButton('СДБ‑дерево', callback_data='SDB')
    )
    bot.send_message(m.chat.id, 'Выберите тип дерева:', reply_markup=kb)

@bot.callback_query_handler(func=lambda c: True)
def cb(c):
    uid = c.from_user.id
    tt = c.data
    if tt in ('AVL', 'Red-Black', 'Splay', 'B', 'DB', 'SDB'):
        st = user_states[uid] = {'tree_type': tt}
        if tt == 'B':
            st['waiting_for'] = 'b_degree'
            bot.send_message(c.message.chat.id, 'Введите порядок B‑дерева (t ≥ 2):')
        else:
            st['waiting_for'] = 'count'
            bot.send_message(c.message.chat.id, 'Введите количество элементов:')
    bot.answer_callback_query(c.id)

@bot.message_handler(func=lambda m: m.from_user.id in Whitelist)
def msg(m):
    uid = m.from_user.id
    if uid not in user_states:
        bot.send_message(m.chat.id, 'Нажмите /start для начала')
        return
    st = user_states[uid]
    try:
        wf = st['waiting_for']
        if wf == 'b_degree':
            t = int(m.text)
            if t < 2:
                bot.send_message(m.chat.id, 't должно быть ≥ 2')
                return
            st['b_degree'] = t
            st['waiting_for'] = 'count'
            bot.send_message(m.chat.id, 'Введите количество элементов:')
        elif wf == 'count':
            n = int(m.text)
            if n <= 0:
                bot.send_message(m.chat.id, 'Количество должно быть > 0')
                return
            st.update({'count': n, 'elements': [], 'current': 0, 'waiting_for': 'elements'})
            tt = st['tree_type']
            if tt == 'B':
                st['btree'] = BTree(st['b_degree'])
            elif tt == 'DB':
                st['dbtree'] = DBTree()
            elif tt == 'SDB':
                st['sdbtree'] = SDBTree()
            else:
                st['root'] = None
                if tt == 'AVL':
                    st['avl'] = AVLTree()
                elif tt == 'Red-Black':
                    st['rb'] = RedBlackTree()
                elif tt == 'Splay':
                    st['splay'] = SplayTree()
            bot.send_message(m.chat.id, f'Теперь вводите элементы. Осталось: {n}\nВведите первый:')
        elif wf == 'elements':
            try:
                k = int(m.text.strip())
                if k in st['elements']:
                    bot.send_message(m.chat.id, 'Такой ключ уже есть, введите другой')
                    return
                st['elements'].append(k)
                st['current'] += 1
                tt = st['tree_type']
                if tt == 'AVL':
                    st['root'] = st['avl'].insert(st['root'], k)
                elif tt == 'Red-Black':
                    st['rb'].insert(k)
                    st['root'] = st['rb'].root
                elif tt == 'Splay':
                    st['root'] = st['splay'].insert(st['root'], k)
                elif tt == 'B':
                    st['btree'].insert(k)
                elif tt == 'DB':
                    st['dbtree'].insert(k)
                elif tt == 'SDB':
                    st['sdbtree'].insert(k)
                # — визулизация
                bobj = st.get('btree') or st.get('dbtree') or st.get('sdbtree')
                img = draw_tree(root=st.get('root'), tree_type=tt, btree=bobj)
                bot.send_photo(m.chat.id, img, caption=','.join(map(str, st['elements'])))
                img.close()
                rem = st['count'] - st['current']
                if rem > 0:
                    bot.send_message(m.chat.id, f'Осталось: {rem}. Введите следующий:')
                else:
                    bot.send_message(m.chat.id, '✅ Готово!')
                    user_states.pop(uid, None)
                    kb = types.InlineKeyboardMarkup()
                    kb.add(types.InlineKeyboardButton('Построить ещё', callback_data='restart'))
                    bot.send_message(m.chat.id, 'Создать новое дерево?', reply_markup=kb)
            except ValueError:
                bot.send_message(m.chat.id, 'Введите целое число')
    except Exception as e:
        bot.send_message(m.chat.id, f'Ошибка: {e}')
        user_states.pop(uid, None)

@bot.callback_query_handler(func=lambda c: c.data == 'restart')
def restart(c):
    cmd_start(c.message)

# ──────────────────────────────────────────────────────────────
# 🚀  ЗАПУСК
# ──────────────────────────────────────────────────────────────
if __name__ == '__main__':
    print('Bot running…')
    bot.polling(none_stop=True)
