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
import time

# Замени на свой токен бота
BOT_TOKEN = "7203298205:AAF3saTT5Z4wqRsh6zdokbNd1nGA0EMxIS0"
bot = telebot.TeleBot(BOT_TOKEN, num_threads=28)
Whitelist = [966064199, 837605460, 1013541709, 1480791192, 1396698393]
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
    """Узел для B‑/DB‑/SDB‑дерева (t = 1)."""
    def __init__(self, is_leaf=True):
        self.keys: list[int] = []
        self.children: list['DBTNode'] = []
        self.is_leaf = is_leaf

class DBTree:
    MAX_KEYS = 2  # при 3 — переполнение

    def __init__(self):
        self.root = DBTNode(is_leaf=True)
        self.steps = []  # Добавляем хранение шагов

    def insert(self, key: int):
        self.steps = []  # Очищаем шаги
        self.steps.append(f"🔵 Вставляем ключ {key}")
        split = self._insert_rec(self.root, key)
        if split:
            mid, right = split
            new_root = DBTNode(is_leaf=False)
            new_root.keys = [mid]
            new_root.children = [self.root, right]
            self.root = new_root
            self.steps.append(f"📈 Создан новый корень с ключом {mid}")
        return self.steps

    def _insert_rec(self, node: DBTNode, key: int):
        if node.is_leaf:
            i = len(node.keys) - 1
            node.keys.append(0)
            while i >= 0 and key < node.keys[i]:
                node.keys[i + 1] = node.keys[i]
                i -= 1
            node.keys[i + 1] = key
            self.steps.append(f"🌿 Вставили {key} в лист")
            return None if len(node.keys) <= self.MAX_KEYS else self._split(node)

        idx = 0
        while idx < len(node.keys) and key > node.keys[idx]:
            idx += 1
        split = self._insert_rec(node.children[idx], key)
        if split:
            mid, right = split
            node.keys.insert(idx, mid)
            node.children.insert(idx + 1, right)
            self.steps.append(f"⚡ Продвинули ключ {mid} вверх")
        return None if len(node.keys) <= self.MAX_KEYS else self._split(node)

    def _split(self, node: DBTNode):
        mid_key = node.keys[1]
        right = DBTNode(is_leaf=node.is_leaf)
        right.keys = node.keys[2:]
        node.keys = node.keys[:1]
        if not node.is_leaf:
            right.children = node.children[2:]
            node.children = node.children[:2]
        self.steps.append(f"💥 Разделили узел, средний ключ: {mid_key}")
        return mid_key, right

class SDBTree(DBTree):
    """Симметричное ДБ (можно заимствовать слева и справа при удалении)."""

    def delete(self, key: int):
        self.steps = []
        self.steps.append(f"🗑️ Удаляем ключ {key}")
        self._delete_rec(self.root, key)
        if not self.root.is_leaf and len(self.root.keys) == 0:
            self.root = self.root.children[0]
            self.steps.append("📉 Корень стал пустым, заменили его единственным ребёнком")
        return self.steps

    def _delete_rec(self, node: DBTNode, key: int):
        i = 0
        while i < len(node.keys) and key > node.keys[i]:
            i += 1

        if i < len(node.keys) and node.keys[i] == key:
            if node.is_leaf:
                node.keys.pop(i)
                self.steps.append(f"🌿 Удалили {key} из листа")
                return
            left, right = node.children[i], node.children[i + 1]
            if len(left.keys) > 1:
                node.keys[i] = self._pop_max(left)
                self.steps.append(f"⬅️ Заменили {key} предшественником")
            elif len(right.keys) > 1:
                node.keys[i] = self._pop_min(right)
                self.steps.append(f"➡️ Заменили {key} преемником")
            else:
                self._merge(node, i)
                self._delete_rec(left, key)
            return

        if node.is_leaf:
            self.steps.append(f"❌ Ключ {key} не найден")
            return

        self._fix_child(node, i)
        self._delete_rec(node.children[i], key)

    def _fix_child(self, parent: DBTNode, idx: int):
        child = parent.children[idx]
        if len(child.keys) > 1:
            return
        if idx > 0 and len(parent.children[idx - 1].keys) > 1:
            left = parent.children[idx - 1]
            borrow = left.keys.pop()
            child.keys.insert(0, parent.keys[idx - 1])
            parent.keys[idx - 1] = borrow
            if not left.is_leaf:
                child.children.insert(0, left.children.pop())
            self.steps.append(f"⬅️ Заимствовали ключ {borrow} у левого брата")
            return
        if idx < len(parent.children) - 1 and len(parent.children[idx + 1].keys) > 1:
            right = parent.children[idx + 1]
            borrow = right.keys.pop(0)
            child.keys.append(parent.keys[idx])
            parent.keys[idx] = borrow
            if not right.is_leaf:
                child.children.append(right.children.pop(0))
            self.steps.append(f"➡️ Заимствовали ключ {borrow} у правого брата")
            return
        if idx > 0:
            self._merge(parent, idx - 1)
        else:
            self._merge(parent, idx)

    def _merge(self, parent: DBTNode, idx: int):
        left = parent.children[idx]
        right = parent.children[idx + 1]
        merge_key = parent.keys.pop(idx)
        left.keys.append(merge_key)
        left.keys.extend(right.keys)
        if not left.is_leaf:
            left.children.extend(right.children)
        parent.children.pop(idx + 1)
        self.steps.append(f"🔗 Слили узлы через ключ {merge_key}")

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

class BTreeNode:
    def __init__(self, is_leaf=False):
        self.keys = []
        self.children = []
        self.is_leaf = is_leaf

class BTree:
    def __init__(self, degree: int):
        self.root = BTreeNode(is_leaf=True)
        self.t = degree
        self.steps = []

    def insert(self, key):
        self.steps = []
        self.steps.append(f"🔵 Вставляем ключ {key}")
        root = self.root
        if len(root.keys) == 2 * self.t - 1:
            new_root = BTreeNode(is_leaf=False)
            new_root.children.append(root)
            self._split_child(new_root, 0)
            self.root = new_root
            self.steps.append("📈 Корень переполнен, создали новый корень")
        self._insert_non_full(self.root, key)
        return self.steps

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
        self.steps.append(f"💥 Разделили узел, средний ключ: {mid}")

    def _insert_non_full(self, node, key):
        i = len(node.keys) - 1
        if node.is_leaf:
            node.keys.append(0)
            while i >= 0 and key < node.keys[i]:
                node.keys[i + 1] = node.keys[i]
                i -= 1
            node.keys[i + 1] = key
            self.steps.append(f"🌿 Вставили {key} в лист")
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
        self.steps = []

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
        self.steps.append(f"🔄 Левый поворот вокруг узла {x.val}")

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
        self.steps.append(f"🔄 Правый поворот вокруг узла {x.val}")

    def insert_fixup(self, z):
        while z.parent.color == 'red':
            if z.parent == z.parent.parent.left:
                y = z.parent.parent.right
                if y.color == 'red':
                    z.parent.color = 'black'
                    y.color = 'black'
                    z.parent.parent.color = 'red'
                    z = z.parent.parent
                    self.steps.append(f"🎨 Перекрасили узлы: {z.parent.val} и дядю в чёрный, {z.val} в красный")
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
                    self.steps.append(f"🎨 Перекрасили узлы: {z.parent.val} и дядю в чёрный, {z.val} в красный")
                else:
                    if z == z.parent.left:
                        z = z.parent
                        self.right_rotate(z)
                    z.parent.color = 'black'
                    z.parent.parent.color = 'red'
                    self.left_rotate(z.parent.parent)
        self.root.color = 'black'

    def insert(self, key):
        self.steps = []
        self.steps.append(f"🔵 Вставляем красный узел {key}")
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
            self.steps.append("🎨 Корень покрасили в чёрный")
            return self.steps

        if z.parent.parent == self.NIL:
            return self.steps

        self.insert_fixup(z)
        return self.steps

class AVLTree:
    def __init__(self):
        self.steps = []

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
        self.steps.append(f"🔄 Правый поворот вокруг узла {y.val}")
        return x

    def rotate_left(self, x):
        y = x.right
        T2 = y.left
        y.left = x
        x.right = T2
        x.height = 1 + max(self.get_height(x.left), self.get_height(x.right))
        y.height = 1 + max(self.get_height(y.left), self.get_height(y.right))
        self.steps.append(f"🔄 Левый поворот вокруг узла {x.val}")
        return y

    def insert(self, root, key):
        self.steps = []
        self.steps.append(f"🔵 Вставляем узел {key}")
        result = self._insert_rec(root, key)
        return result, self.steps

    def _insert_rec(self, root, key):
        if not root:
            return TreeNode(key)
        
        if key < root.val:
            root.left = self._insert_rec(root.left, key)
        elif key > root.val:
            root.right = self._insert_rec(root.right, key)
        else:
            return root

        root.height = 1 + max(self.get_height(root.left), self.get_height(root.right))
        balance = self.get_balance(root)

        # Проверяем нарушения балансировки
        if balance > 1:
            self.steps.append(f"⚠️ Дисбаланс в узле {root.val} (левое поддерево тяжелее)")
            if key < root.left.val:
                self.steps.append("📐 Случай LL - нужен правый поворот")
                return self.rotate_right(root)
            else:
                self.steps.append("📐 Случай LR - нужен левый поворот, затем правый")
                root.left = self.rotate_left(root.left)
                return self.rotate_right(root)
        
        if balance < -1:
            self.steps.append(f"⚠️ Дисбаланс в узле {root.val} (правое поддерево тяжелее)")
            if key > root.right.val:
                self.steps.append("📐 Случай RR - нужен левый поворот")
                return self.rotate_left(root)
            else:
                self.steps.append("📐 Случай RL - нужен правый поворот, затем левый")
                root.right = self.rotate_right(root.right)
                return self.rotate_left(root)
        
        return root

class SplayTree:
    def __init__(self):
        self.steps = []

    def right_rotate(self, x):
        y = x.left
        x.left = y.right
        y.right = x
        self.steps.append(f"🔄 Правый поворот (zag) вокруг узла {x.val}")
        return y

    def left_rotate(self, x):
        y = x.right
        x.right = y.left
        y.left = x
        self.steps.append(f"🔄 Левый поворот (zig) вокруг узла {x.val}")
        return y

    def splay(self, root, key):
        if not root or root.val == key:
            return root

        if root.val > key:
            if not root.left:
                return root
            
            if root.left.val > key:
                self.steps.append("📐 Zig-Zig случай (левый-левый)")
                root.left.left = self.splay(root.left.left, key)
                root = self.right_rotate(root)
            elif root.left.val < key:
                self.steps.append("📐 Zig-Zag случай (левый-правый)")
                root.left.right = self.splay(root.left.right, key)
                if root.left.right:
                    root.left = self.left_rotate(root.left)
            
            return self.right_rotate(root) if root.left else root
        else:
            if not root.right:
                return root
            
            if root.right.val > key:
                self.steps.append("📐 Zag-Zig случай (правый-левый)")
                root.right.left = self.splay(root.right.left, key)
                if root.right.left:
                    root.right = self.right_rotate(root.right)
            elif root.right.val < key:
                self.steps.append("📐 Zag-Zag случай (правый-правый)")
                root.right.right = self.splay(root.right.right, key)
                root = self.left_rotate(root)
            
            return self.left_rotate(root) if root.right else root

    def insert(self, root, key):
        self.steps = []
        self.steps.append(f"🔵 Вставляем узел {key}")
        
        if not root:
            return TreeNode(key), self.steps
        
        root = self.splay(root, key)
        
        if root.val == key:
            return root, self.steps
        
        new_node = TreeNode(key)
        
        if root.val > key:
            self.steps.append(f"➡️ {key} < {root.val}, делаем {key} новым корнем")
            new_node.right = root
            new_node.left = root.left
            root.left = None
        else:
            self.steps.append(f"⬅️ {key} > {root.val}, делаем {key} новым корнем")
            new_node.left = root
            new_node.right = root.right
            root.right = None
        
        return new_node, self.steps

def draw_tree(root=None, tree_type="Binary", btree=None):
    plt.ioff()
    fig, ax = plt.subplots(figsize=(14, 10))
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
    if not node:
        return

    key_positions = _draw_node_keys_separately(ax, node, x, y)
    
    if not node.is_leaf:
        _draw_children_with_connections(ax, node, key_positions, y, width, tree_kind)

def _draw_node_keys_separately(ax, node, center_x, y):
    keys = node.keys
    if not keys:
        return []
    
    key_spacing = 0.8
    total_width = (len(keys) - 1) * key_spacing
    start_x = center_x - total_width / 2
    
    key_positions = []
    
    for i, key in enumerate(keys):
        key_x = start_x + i * key_spacing
        key_positions.append((key_x, y, key))
        
        color = 'lightgreen' if node.is_leaf else 'lightblue'
        ax.add_patch(plt.Circle((key_x, y), 0.25, 
                               color=color, ec='black', linewidth=2))
        ax.text(key_x, y, str(key), 
               ha='center', va='center', 
               fontsize=10, fontweight='bold')
    
    for i in range(len(key_positions) - 1):
        x1, y1, _ = key_positions[i]
        x2, y2, _ = key_positions[i + 1]
        
        ax.plot([x1 + 0.25, x2 - 0.25], [y1, y2], 
               linestyle='--', color='red', linewidth=2, alpha=0.8)
    
    return key_positions

def _draw_children_with_connections(ax, node, key_positions, parent_y, width, tree_kind):
    if len(node.children) == 0:
        return
    
    child_y = parent_y - 2.0
    child_spacing = width / len(node.children)
    
    children_positions = []
    for i, child in enumerate(node.children):
        child_x = key_positions[0][0] - width/2 + child_spacing/2 + i * child_spacing
        children_positions.append((child_x, child_y, child))
    
    for i, (child_x, child_y, child) in enumerate(children_positions):
        if i < len(key_positions):
            key_x, key_y, _ = key_positions[i] if i < len(key_positions) else key_positions[-1]
        else:
            key_x, key_y, _ = key_positions[-1]
        
        if i > 0 and i <= len(key_positions):
            key_x, key_y, _ = key_positions[i-1]
        
        ax.plot([key_x, child_x], [key_y - 0.25, child_y + 0.3], 
               'k-', linewidth=2)
        
        draw_btree(ax, child, child_x, child_y, child_spacing * 0.8, tree_kind)

@bot.message_handler(commands=['start'], func=lambda m: m.from_user.id in Whitelist)
def cmd_start(m):
    if m.from_user.id == 966064199:
        bot.send_message(m.chat.id, "Приветствую, Босс")
    if m.from_user.id == 837605460:
        bot.send_photo(m.chat_id, 'sanya.png')
        bot.send_message(m.chat.id, "Ай саул брат, как поживаешь брат?")
    if m.from_user.id == 1013541709:
        bot.send_photo(m.chat_id, 'vladick.png')
        bot.send_message(m.chat.id, "HEEY! Wasup my black broooo!!")
    if m.from_user.id == 1480791192:
        bot.send_photo(m.chat_id, 'bobik.png')
        bot.send_message(m.chat.id, "... Гав гав моему боби.. Блять! Привет бобанчик.")
    
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
# Основной обработчик сообщений - добавляем фильтр для исключения callback queries
@bot.message_handler(func=lambda m: m.from_user.id in Whitelist and m.content_type == 'text')
def msg(m):
    uid = m.from_user.id
    if uid not in user_states:
        bot.send_message(m.chat.id, 'Нажмите /start для начала')
        return
    st = user_states[uid]
    try:
        wf = st['waiting_for']
        if wf == 'delete_element':
            handle_delete_element(m, st)
        elif wf == 'b_degree':
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
            st.update({'count': n, 'elements': [], 'current_index': 0})
            
            # Инициализируем структуру данных в зависимости от типа дерева
            tree_type = st['tree_type']
            if tree_type == 'AVL':
                st['tree'] = AVLTree()
                st['root'] = None
            elif tree_type == 'Red-Black':
                st['tree'] = RedBlackTree()
            elif tree_type == 'Splay':
                st['tree'] = SplayTree()
                st['root'] = None
            elif tree_type == 'B':
                st['tree'] = BTree(st['b_degree'])
            elif tree_type == 'DB':
                st['tree'] = DBTree()
            elif tree_type == 'SDB':
                st['tree'] = SDBTree()
            
            st['waiting_for'] = 'elements'
            bot.send_message(m.chat.id, f'Введите первый элемент:')
        
        elif wf == 'elements':
            elements = list(map(int, m.text.split()))
            # if len(elements) != st['count']:
            #     bot.send_message(m.chat.id, f'Нужно ввести ровно {st["count"]} элементов')
            #     return
            
            st['elements'] = elements
            st['current_index'] = 0
            
            # Начинаем процесс поэтапной вставки
            insert_next_element(m.chat.id, uid)
            
    except ValueError:
        bot.send_message(m.chat.id, 'Введите корректное число')
    except Exception as e:
        bot.send_message(m.chat.id, f'Ошибка: {str(e)}')

def insert_next_element(chat_id, uid):
    """Вставляет следующий элемент и показывает промежуточные результаты"""
    st = user_states[uid]
    
    if st['current_index'] >= len(st['elements']):
        bot.send_message(chat_id, '✅ Все элементы вставлены!')
        return
    
    element = st['elements'][st['current_index']]
    tree_type = st['tree_type']
    
    bot.send_message(chat_id, f"🔄 Вставляем элемент: {element}")
    
    try:
        if tree_type == 'AVL':
            st['root'], steps = st['tree'].insert(st['root'], element)
            send_steps_with_images(chat_id, uid, steps, tree_type)
            
        elif tree_type == 'Red-Black':
            steps = st['tree'].insert(element)
            send_steps_with_images(chat_id, uid, steps, tree_type)
            
        elif tree_type == 'Splay':
            st['root'], steps = st['tree'].insert(st['root'], element)
            send_steps_with_images(chat_id, uid, steps, tree_type)
            
        elif tree_type in ('B', 'DB', 'SDB'):
            if tree_type == 'B':
                steps = st['tree'].insert(element)
            elif tree_type == 'DB':
                steps = st['tree'].insert(element)
            elif tree_type == 'SDB':
                steps = st['tree'].insert(element)
            
            send_btree_steps_with_images(chat_id, uid, steps, tree_type)
        
        st['current_index'] += 1
        
        # Кнопка для продолжения или завершения
        kb = types.InlineKeyboardMarkup()
        if st['current_index'] < len(st['elements']):
            kb.add(types.InlineKeyboardButton('➡️ Вставить следующий элемент', 
                                            callback_data=f'next_{uid}'))
        kb.add(types.InlineKeyboardButton('🏁 Завершить', callback_data=f'finish_{uid}'))
        
        remaining = len(st['elements']) - st['current_index']
        if remaining > 0:
            bot.send_message(chat_id, f"Осталось элементов: {remaining}", reply_markup=kb)
        else:
            bot.send_message(chat_id, "Вставка завершена!", reply_markup=kb)
            
    except Exception as e:
        bot.send_message(chat_id, f'Ошибка при вставке: {str(e)}')

def send_steps_with_images(chat_id, uid, steps, tree_type):
    """Отправляет шаги с изображениями для бинарных деревьев"""
    st = user_states[uid]
    
    # Отправляем текстовые шаги
    for step in steps:
        bot.send_message(chat_id, step)
        time.sleep(0.5)  # Небольшая задержка для читаемости
    
    # Отправляем изображение текущего состояния дерева
    try:
        if tree_type == 'AVL' or tree_type == 'Splay':
            img_buf = draw_tree(st['root'], tree_type)
        elif tree_type == 'Red-Black':
            img_buf = draw_tree(st['tree'].root, tree_type)
        
        if img_buf:
            bot.send_photo(chat_id, img_buf)
            img_buf.close()
    except Exception as e:
        bot.send_message(chat_id, f'Ошибка при создании изображения: {str(e)}')

def send_btree_steps_with_images(chat_id, uid, steps, tree_type):
    """Отправляет шаги с изображениями для B-деревьев"""
    st = user_states[uid]
    
    # Отправляем текстовые шаги
    for step in steps:
        bot.send_message(chat_id, step)
        time.sleep(0.5)  # Небольшая задержка для читаемости
    
    # Отправляем изображение текущего состояния дерева
    try:
        img_buf = draw_tree(btree=st['tree'], tree_type=tree_type)
        if img_buf:
            bot.send_photo(chat_id, img_buf)
            img_buf.close()
    except Exception as e:
        bot.send_message(chat_id, f'Ошибка при создании изображения: {str(e)}')

# ИСПРАВЛЕННЫЙ обработчик для кнопок - добавляем проверку whitelist
@bot.callback_query_handler(func=lambda c: c.from_user.id in Whitelist and 
                           (c.data.startswith('next_') or c.data.startswith('finish_')))
def handle_continue_finish(c):
    uid = c.from_user.id
    print(f"Callback received: {c.data} from user {uid}")  # Отладка
    
    try:
        if c.data.startswith('next_'):
            target_uid = int(c.data.split('_')[1])
            print(f"Next button: target_uid={target_uid}, current_uid={uid}")  # Отладка
            if uid == target_uid and uid in user_states:
                print(f"Inserting next element for user {uid}")  # Отладка
                # Показываем пользователю что кнопка нажата
                bot.answer_callback_query(c.id, "⏳ Обрабатываем...")
                insert_next_element(c.message.chat.id, uid)
            else:
                bot.answer_callback_query(c.id, "❌ Ошибка: неверный пользователь", show_alert=True)
                bot.send_message(c.message.chat.id, "Ошибка: неверный пользователь или сессия")
                
        elif c.data.startswith('finish_'):
            target_uid = int(c.data.split('_')[1])
            if uid == target_uid:
                bot.answer_callback_query(c.id, "✅ Сессия завершена")
                if uid in user_states:
                    del user_states[uid]
                bot.send_message(c.message.chat.id, '✅ Сессия завершена. Нажмите /start для новой сессии.')
            else:
                bot.answer_callback_query(c.id, "❌ Ошибка доступа", show_alert=True)
    
    except Exception as e:
        print(f"Error in callback handler: {e}")
        bot.answer_callback_query(c.id, f"❌ Ошибка: {str(e)}", show_alert=True)
        bot.send_message(c.message.chat.id, f'Ошибка обработки: {str(e)}')

# Добавляем обработчик для команды удаления (только для SDB-дерева)
@bot.message_handler(commands=['delete'], func=lambda m: m.from_user.id in Whitelist)
def cmd_delete(m):
    uid = m.from_user.id
    if uid not in user_states:
        bot.send_message(m.chat.id, 'Нажмите /start для начала')
        return
    
    st = user_states[uid]
    if st['tree_type'] != 'SDB':
        bot.send_message(m.chat.id, 'Удаление доступно только для СДБ-дерева')
        return
    
    st['waiting_for'] = 'delete_element'
    bot.send_message(m.chat.id, 'Введите элемент для удаления:')

def handle_delete_element(m, st):
    """Обработка удаления элемента из SDB-дерева"""
    try:
        element = int(m.text)
        steps = st['tree'].delete(element)
        
        bot.send_message(m.chat.id, f"🗑️ Удаляем элемент: {element}")
        
        # Отправляем шаги удаления
        for step in steps:
            bot.send_message(m.chat.id, step)
            time.sleep(0.5)
        
        # Отправляем изображение результата
        try:
            img_buf = draw_tree(btree=st['tree'], tree_type='SDB')
            if img_buf:
                bot.send_photo(m.chat.id, img_buf)
                img_buf.close()
        except Exception as e:
            bot.send_message(m.chat.id, f'Ошибка при создании изображения: {str(e)}')
        
        st['waiting_for'] = 'elements'  # Возвращаемся к режиму вставки
        
    except ValueError:
        bot.send_message(m.chat.id, 'Введите корректное число')

# Функция для отладки состояний пользователей
def debug_user_state(uid):
    """Выводит текущее состояние пользователя для отладки"""
    if uid in user_states:
        st = user_states[uid]
        print(f"User {uid} state: {st}")
    else:
        print(f"User {uid} not found in states")
@bot.message_handler(func=lambda mess: mess.from_user.id in [770025733])
def go_away(message):
    bot.send_message(message, "РЫБИН ПОШЕЛ НАХУЙ!!!!")
# Запуск бота
if __name__ == "__main__":
    print("Бот запущен...")
    bot.infinity_polling(none_stop=True)