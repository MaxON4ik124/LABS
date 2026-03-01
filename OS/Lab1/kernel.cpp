// kernel.cpp (32-bit freestanding, VGA + keyboard polling)

// extern "C" void kmain();

// Надёжная точка входа для загрузчика: jmp/call в kmain
__asm__(
    ".code32\n"
    ".global _start\n"
    "_start:\n"
    "    call kmain\n"
    "1:  hlt\n"
    "    jmp 1b\n"
);

#define VIDEO_BUF_PTR 0xB8000
static volatile unsigned short* const VGA = (unsigned short*)VIDEO_BUF_PTR;
static const int VGA_W = 80;
static const int VGA_H = 25;

// --- I/O портов ---
static inline void outb(unsigned short port, unsigned char val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// --- VGA вывод ---
static inline unsigned short vga_cell(unsigned char ch, unsigned char color) {
    return (unsigned short)((color << 8) | ch);
}

static void vga_put_at(int row, int col, unsigned char ch, unsigned char color) {
    if (row < 0 || row >= VGA_H || col < 0 || col >= VGA_W) return;
    VGA[row * VGA_W + col] = vga_cell(ch, color);
}

static void vga_write_str(int row, int col, unsigned char color, const char* s) {
    int c = col;
    while (*s && c < VGA_W) {
        vga_put_at(row, c, (unsigned char)*s, color);
        ++c;
        ++s;
    }
}

static void vga_clear_row(int row, unsigned char color) {
    for (int col = 0; col < VGA_W; ++col) {
        vga_put_at(row, col, ' ', color);
    }
}

// --- Простейшая таблица scancode set 1 -> ASCII (US), без Shift ---
static const char sc_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', // 0x0E = backspace
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,   // 0x1C = enter
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*',  0,  ' ',
    0,0,0,0,0,0,0,0,0,0, // F-keys и т.п. игнорируем
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

// Ждём следующий scancode (polling)
static unsigned char kbd_read_scancode() {
    // статус контроллера: бит0=1 => есть байт в буфере вывода
    while ((inb(0x64) & 0x01) == 0) {
        asm volatile ("hlt"); // экономим CPU, QEMU нормально
    }
    return inb(0x60);
}

static bool kbd_scancode_to_char(unsigned char sc, char& out) {
    if (sc & 0x80) return false;              // key release
    char c = sc_to_ascii[sc & 0x7F];
    if (!c) return false;                     // не печатный/не поддержан
    out = c;
    return true;
}

// Ввод строки с экранированием в VGA (без Shift/Caps), поддержка backspace, enter
static void read_line(char* buf, int max_len, int row, int col, unsigned char color) {
    int len = 0;
    int cur = col;

    // очистим остаток строки справа от курсора
    for (int i = col; i < VGA_W; ++i) vga_put_at(row, i, ' ', color);

    while (true) {
        unsigned char sc = kbd_read_scancode();
        char ch;
        if (!kbd_scancode_to_char(sc, ch)) continue;

        if (ch == '\n') { // Enter
            buf[len] = '\0';
            return;
        }

        if (ch == '\b') { // Backspace
            if (len > 0) {
                --len;
                --cur;
                vga_put_at(row, cur, ' ', color);
            }
            continue;
        }

        // фильтр: только "видимые" и пока есть место
        if (ch >= 32 && ch <= 126 && len < (max_len - 1) && cur < VGA_W) {
            buf[len++] = ch;
            vga_put_at(row, cur++, (unsigned char)ch, color);
        }
    }
}

extern "C" int kmain()
{
    // Немного “UI”
    vga_clear_row(5, 0x07);
    vga_clear_row(6, 0x07);
    vga_clear_row(7, 0x07);
    vga_clear_row(8, 0x07);
    vga_clear_row(9, 0x07);
    vga_clear_row(10,0x07);

    const char* hello = "Welcome to HelloWorldOS (gcc edition)!";
    vga_write_str(6, 0, 0x0A, hello);

    const char* prompt = "Enter your name: ";
    vga_write_str(8, 0, 0x0F, prompt);

    char name[32];
    read_line(name, (int)sizeof(name), 8, 17, 0x0F);

    // Вывод "Hello, {name}"
    vga_clear_row(10, 0x07);
    vga_write_str(10, 0, 0x0E, "Hello, ");
    vga_write_str(10, 7, 0x0E, name);

    while (1) {
        asm volatile ("hlt");
    }
    return 0;
}