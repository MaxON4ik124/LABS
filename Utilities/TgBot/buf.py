
def draw_tree(root=None, tree_type="Binary", btree=None):
    plt.ioff()
    fig, ax = plt.subplots(figsize=(12, 8))
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 8)
    ax.axis('off')
    try:
        if tree_type in ("B", "DB", "SDB") and btree:
            draw_btree(ax, btree.root, 5, 7, 4, tree_type)
        elif root:
            draw_binary_tree(ax, root, 5, 7, 2, tree_type)
        plt.title(f"{tree_type}-дерево", fontsize=16)
        buf = BytesIO()
        plt.savefig(buf, format='png', bbox_inches='tight', dpi=300)
        buf.seek(0)
        return buf
    finally:
        plt.close(fig)
        plt.clf()
        gc.collect()
# --- Двоичное дерево (AVL / RB / Splay) ---

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

# ---------- draw_btree ----------


def draw_btree(ax, node, x, y, width, tree_kind="B"):
    """
    Рекурсивная отрисовка B / DB / SDB‑дерева.
    • Добавлены пунктирные горизонтальные ссылки между соседями.
    • tree_kind нужен только для совместимости, но логика одинаковая.
    """
    if not node:
        return

    # 1️⃣ узел
    node_w = max(len(node.keys) * 0.6, 0.6)
    ax.add_patch(patches.Rectangle((x - node_w / 2, y - 0.2),
                                   node_w, 0.4,
                                   linewidth=2, edgecolor='black',
                                   facecolor='lightgreen'))
    for i, k in enumerate(node.keys):
        ax.text(x - node_w / 2 + 0.3 + i * 0.6,
                y, str(k),
                ha='center', va='center',
                fontsize=10, fontweight='bold')

    # 2️⃣ дети
    if node.is_leaf:
        return

    child_w = width / len(node.children)
    child_centers = []
    for i, child in enumerate(node.children):
        cx = x - width / 2 + child_w / 2 + i * child_w
        cy = y - 1
        child_centers.append((cx, cy))
        # связь родитель‑ребёнок
        ax.plot([x, cx], [y - 0.2, cy + 0.2], 'k-', linewidth=2)
        draw_btree(ax, child, cx, cy, child_w * 0.8, tree_kind)

    # 3️⃣ пунктир между соседями (горизонтальные ссылки «братьев»)
    for (cx1, cy), (cx2, _) in zip(child_centers, child_centers[1:]):
        ax.plot([cx1, cx2],
                [cy, cy],
                linestyle='--', linewidth=1.6)