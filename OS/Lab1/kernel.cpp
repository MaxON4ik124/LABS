// kernel.cpp
// 32-bit freestanding kernel: VGA text console + PS/2 keyboard polling

__asm__(
    ".code32\n"
    ".global _start\n"
    "_start:\n"
    "    cli\n"
    "    call kmain\n"
    "1:  hlt\n"
    "    jmp 1b\n"
);

extern "C" void kmain();

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;

static constexpr u32 VIDEO_BUF_PTR = 0xB8000;
static volatile u16* const VGA = (u16*)VIDEO_BUF_PTR;

static constexpr int VGA_W = 80;
static constexpr int VGA_H = 25;

// -------------------------
// Port I/O
// -------------------------
static inline void outb(u16 port, u8 val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline u8 inb(u16 port) {
    u8 ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// -------------------------
// Tiny libc-like helpers
// -------------------------
static int strlen(const char* s) {
    int n = 0;
    while (s[n]) ++n;
    return n;
}

static bool streq(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return false;
        ++a;
        ++b;
    }
    return *a == *b;
}

static void strcpy(char* dst, const char* src) {
    while (*src) {
        *dst++ = *src++;
    }
    *dst = '\0';
}

// -------------------------
// VGA console
// -------------------------
static inline u16 vga_cell(char ch, u8 color) {
    return (u16)(((u16)color << 8) | (u8)ch);
}

struct Console {
    int row = 0;
    int col = 0;
    u8 color = 0x07;

    void move_hw_cursor() {
        u16 pos = (u16)(row * VGA_W + col);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (u8)(pos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (u8)((pos >> 8) & 0xFF));
    }

    void clear_row(int r, u8 clr) {
        if (r < 0 || r >= VGA_H) return;
        for (int c = 0; c < VGA_W; ++c) {
            VGA[r * VGA_W + c] = vga_cell(' ', clr);
        }
    }

    void clear() {
        for (int r = 0; r < VGA_H; ++r) {
            clear_row(r, color);
        }
        row = 0;
        col = 0;
        move_hw_cursor();
    }

    void scroll_if_needed() {
        if (row < VGA_H) return;

        for (int r = 1; r < VGA_H; ++r) {
            for (int c = 0; c < VGA_W; ++c) {
                VGA[(r - 1) * VGA_W + c] = VGA[r * VGA_W + c];
            }
        }
        clear_row(VGA_H - 1, color);
        row = VGA_H - 1;
    }

    void putc(char ch) {
        if (ch == '\n') {
            col = 0;
            ++row;
            scroll_if_needed();
            move_hw_cursor();
            return;
        }

        if (ch == '\r') {
            col = 0;
            move_hw_cursor();
            return;
        }

        if (ch == '\b') {
            if (col > 0) {
                --col;
                VGA[row * VGA_W + col] = vga_cell(' ', color);
            }
            move_hw_cursor();
            return;
        }

        if (ch == '\t') {
            int next = (col + 4) & ~3;
            while (col < next) putc(' ');
            return;
        }

        VGA[row * VGA_W + col] = vga_cell(ch, color);
        ++col;

        if (col >= VGA_W) {
            col = 0;
            ++row;
            scroll_if_needed();
        }

        move_hw_cursor();
    }

    void print(const char* s) {
        while (*s) putc(*s++);
    }

    void println(const char* s) {
        print(s);
        putc('\n');
    }

    void set_color(u8 clr) {
        color = clr;
    }
};

static Console console;

// -------------------------
// Keyboard (PS/2, set 1, polling)
// -------------------------
struct KeyboardState {
    bool left_shift  = false;
    bool right_shift = false;
    bool caps_lock   = false;

    bool shift() const {
        return left_shift || right_shift;
    }
};

static KeyboardState kbd;

static const char keymap[128] = {
    0,   27,  '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*',  0, ' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static const char keymap_shift[128] = {
    0,   27,  '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*',  0, ' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static u8 kbd_read_scancode() {
    while ((inb(0x64) & 0x01) == 0) {
        asm volatile ("pause");
    }
    return inb(0x60);
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool kbd_translate_scancode(u8 sc, char& out) {
    out = 0;

    // extended prefix пока игнорируем
    if (sc == 0xE0) {
        (void)kbd_read_scancode();
        return false;
    }

    bool released = (sc & 0x80) != 0;
    u8 code = sc & 0x7F;

    switch (code) {
        case 0x2A: // left shift
            kbd.left_shift = !released;
            return false;
        case 0x36: // right shift
            kbd.right_shift = !released;
            return false;
        case 0x3A: // caps lock
            if (!released) kbd.caps_lock = !kbd.caps_lock;
            return false;
        default:
            break;
    }

    if (released) return false;
    if (code >= 128) return false;

    char c = kbd.shift() ? keymap_shift[code] : keymap[code];
    if (!c) return false;

    if (is_alpha(c)) {
        bool upper = kbd.shift() ^ kbd.caps_lock;
        if (upper) {
            if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
        } else {
            if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        }
    }

    out = c;
    return true;
}

// -------------------------
// Line input
// -------------------------
static void read_line(char* buf, int max_len) {
    int len = 0;

    while (true) {
        char ch;
        if (!kbd_translate_scancode(kbd_read_scancode(), ch)) {
            continue;
        }

        if (ch == '\n') {
            buf[len] = '\0';
            console.putc('\n');
            return;
        }

        if (ch == '\b') {
            if (len > 0) {
                --len;
                console.putc('\b');
            }
            continue;
        }

        if (ch >= 32 && ch <= 126 && len < max_len - 1) {
            buf[len++] = ch;
            console.putc(ch);
        }
    }
}

// -------------------------
// Command loop
// -------------------------
static void print_banner() {
    console.set_color(0x0A);
    console.println("HelloWorldOS");
    console.set_color(0x07);
    console.println("------------------------------");
    console.println("Commands: help, clear, about");
    console.println("");
}

static void handle_command(const char* cmd) {
    if (streq(cmd, "")) {
        return;
    }

    if (streq(cmd, "help")) {
        console.println("Available commands:");
        console.println("  help  - show this help");
        console.println("  clear - clear screen");
        console.println("  about - show system info");
        return;
    }

    if (streq(cmd, "clear")) {
        console.clear();
        return;
    }

    if (streq(cmd, "about")) {
        console.println("HelloWorldOS kernel");
        console.println("Mode: 32-bit protected mode");
        console.println("Keyboard: PS/2 polling");
        console.println("Display: VGA text mode");
        return;
    }

    console.set_color(0x0C);
    console.print("Unknown command: ");
    console.println(cmd);
    console.set_color(0x07);
}

extern "C" void kmain() {
    console.set_color(0x07);
    console.clear();

    print_banner();

    char line[64];

    while (true) {
        console.set_color(0x0F);
        console.print("> ");
        console.set_color(0x07);

        read_line(line, sizeof(line));
        handle_command(line);
    }
}