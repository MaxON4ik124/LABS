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

static constexpr unsigned int BOOT_COLOR_ADDR = 0x500;

static unsigned char load_boot_color() 
{
    volatile unsigned char* p = (volatile unsigned char*)BOOT_COLOR_ADDR;
    unsigned char c = *p;

    switch (c) 
    {
        case 0x07:
        case 0x0F:
        case 0x0E:
        case 0x09:
        case 0x0C:
        case 0x0A:
            return c;
        default:
            return 0x07;
    }
}




static inline void outb(u16 port, u8 val) 
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline void outw(u16 port, u16 val)
{
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline u8 inb(u16 port) 
{
    u8 ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}


static int strlen(const char* s) 
{
    int n = 0;
    while (s[n]) ++n;
    return n;
}

static bool streq(const char* a, const char* b) 
{
    while (*a && *b) 
    {

        if (*a != *b) return false;
        ++a;
        ++b;
    }
    return *a == *b;
}

static void strcpy(char* dst, const char* src) 
{
    while (*src) 
    {
        *dst++ = *src++;
    }
    *dst = '\0';
}
int detect_leap(int year)
{
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}
static long long pow(int base, int index)
{
    long long res = 1;
    for(int i = 0;i < index;i++)
        res *= base;
    return res;
}
static long long atoi(char* cnt)
{
    long long res = 0;
    int i = 0;
    short neg = 0;
    if(cnt[0] == '-')
    {
        i++;
        neg = 1;
    }
    for(i;i < strlen(cnt);i++)
        res += (cnt[i] - '0') * pow(10, strlen(cnt)-i-1);
    if(neg) res *= -1;
    return res;
}
static char conv_to_digit_chr(int digit)
{
    if(digit < 10)
        return '0' + digit;
    else
        return 'a' + digit - 10;
}
static int conv_to_digit_int(char digit)
{
    if('0' <= digit && digit <= '9')
        return digit - '0';
    else
        return digit - 'a' + 10;
}
static int parse_args(char* line, char* argv[], int max_args)
{
    int argc = 0;
    while (*line && argc < max_args)
    {
        while(*line == ' ') *line++;
        if(*line == '\0') break;
        argv[argc++] = line;
        while(*line && *line != ' ') *line++;
        if(*line)
        {
            *line = '\0';
            line++;
        }
    }
    return argc;
}
static char is_valid(char* cnt, char* base)
{
    int base_ = (int)atoi(base);
    char base_digit;
    if(base_ > 9)
        base_digit = 'a' + base_ - 10;
    else
        base_digit = '0' + base_;
    while(*cnt != '\0')
    {
        if(*cnt >= base_digit) return *cnt;
        ++cnt;
    }
    return '0';
}
static long long conv10(char* line, char* base_src)
{
    int line_ = (int)atoi(line);
    int base = (int)atoi(base_src);
    long long res = 0;
    short neg = 0;
    if(line[0] == '-') neg = 1;
    short order = strlen(line) - neg;
    for(int i = neg;i < order+neg;i++)
        res += conv_to_digit_int(line[i]) * pow(base, order-i-1+neg);
    if(neg == 1) res *= -1;
    return res;
}
static void nsconv(int line, char* base_dest, char* res)
{
    int line_ = line;
    int base_ = (int)atoi(base_dest);
    short neg = 0;
    int order = 0;
    if(line_ < 0) 
    {
        neg = 1;
        res[0] = '-';
        line *= -1;
        
    }
    while(line_ != 0)
    {
        line_ /= base_;
        order++;
    }
    
    for(int i = order + neg - 1;i >= neg;i--)
    {
        res[i] = conv_to_digit_chr(line % base_);
        line /= base_;
    }
    res[order + neg] = '\0';
}
void write2(char* buf, int v)
{
    buf[0] = '0' + (v / 10);
    buf[1] = '0' + (v % 10);
};
void get_time(char* timee, int mode, char* res)
{
    long long time_ = atoi(timee);
    int time;
    int before_posix = 0;
    if(mode == 0) time = (int)atoi(timee);
    if(mode == 1 && time_ > 11644473600) time = atoi(timee) - 11644473600;
    if(mode == 1 && time_ <= 11644473600)
    {
        time = (int)atoi(timee);
        before_posix = 1;
    }
    int yr, mth, dy, hr, mnt, sc;
    mth = 0;
    sc = time % 60;
    time /= 60;
    mnt = time % 60;
    time /= 60;
    hr = time % 24;
    time /= 24;
    yr = (before_posix) ? 1601 : 1970;
    int days_in_yr;
    while(1)
    {
        days_in_yr = detect_leap(yr) ? 366 : 365;
        if(time >= days_in_yr)
        {
            yr++;
            time -= days_in_yr;
        }
        else
            break;
    }
    static int monthdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    for(int i = 0;i < 12;i++)
    {
        int days_in_month = monthdays[i];
        if(i == 1 && detect_leap(yr))
            days_in_month = 29;
        if(time >= days_in_month)
        {
            time -= days_in_month;
            mth++;
        }
        else
            break;
    }
    mth++;
    dy = time + 1;
    write2(res, dy);
    res[2] = '.';
    write2(res+3, mth);
    res[5] = '.';
    res[6] = '0' + (yr / 1000) % 10;
    res[7] = '0' + (yr / 100)  % 10;
    res[8] = '0' + (yr / 10)   % 10;
    res[9] = '0' + (yr % 10);
    res[10] = ' ';
    write2(res + 11, hr);
    res[13] = ':';
    write2(res + 14, mnt);
    res[16] = ':';
    write2(res + 17, sc);
    res[19] = '\0';
}
static inline u16 vga_cell(char ch, u8 color) 
{
    return (u16)(((u16)color << 8) | (u8)ch);
}

struct Console 
{
    int row = 0;
    int col = 0;
    u8 color = 0x07;

    void shutdown()
    {
        outw(0x604, 0x2000);
    }

    void move_hw_cursor() 
    {
        u16 pos = (u16)(row * VGA_W + col);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (u8)(pos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (u8)((pos >> 8) & 0xFF));
    }

    void clear_row(int r, u8 clr) 
    {
        if (r < 0 || r >= VGA_H) return;
        for (int c = 0; c < VGA_W; ++c) 
        {
            VGA[r * VGA_W + c] = vga_cell(' ', clr);
        }
    }

    void clear() 
    {
        for (int r = 0; r < VGA_H; ++r) 
        {
            clear_row(r, color);
        }
        row = 0;
        col = 0;
        move_hw_cursor();
    }

    void scroll_if_needed() 
    {
        if (row < VGA_H) return;

        for (int r = 1; r < VGA_H; ++r) 
        {
            for (int c = 0; c < VGA_W; ++c) 
            {
                VGA[(r - 1) * VGA_W + c] = VGA[r * VGA_W + c];
            }
        }
        clear_row(VGA_H - 1, color);
        row = VGA_H - 1;
    }

    void putc(char ch) {
        if (ch == '\n') 
        {
            col = 0;
            ++row;
            scroll_if_needed();
            move_hw_cursor();
            return;
        }

        if (ch == '\r') 
        {
            col = 0;
            move_hw_cursor();
            return;
        }

        if (ch == '\b') 
        {
            if (col > 0) 
            {
                --col;
                VGA[row * VGA_W + col] = vga_cell(' ', color);
            }
            move_hw_cursor();
            return;
        }

        if (ch == '\t') 
        {
            int next = (col + 4) & ~3;
            while (col < next) putc(' ');
            return;
        }

        VGA[row * VGA_W + col] = vga_cell(ch, color);
        ++col;

        if (col >= VGA_W) 
        {
            col = 0;
            ++row;
            scroll_if_needed();
        }

        move_hw_cursor();
    }

    void print(const char* s) 
    {
        while (*s) putc(*s++);
    }

    void println(const char* s) 
    {
        print(s);
        putc('\n');
    }

    void set_color(u8 clr) 
    {
        color = clr;
    }
};

static Console console;


struct KeyboardState 
{
    bool left_shift  = false;
    bool right_shift = false;
    bool caps_lock   = false;

    bool shift() const 
    {
        return left_shift || right_shift;
    }
};

static KeyboardState kbd;

static const char keymap[128] = 
{
    0,   27,  '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*',  0, ' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static const char keymap_shift[128] = 
{
    0,   27,  '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*',  0, ' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static u8 kbd_read_scancode() 
{
    while ((inb(0x64) & 0x01) == 0) {
        asm volatile ("pause");
    }
    return inb(0x60);
}

static bool is_alpha(char c) 
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool kbd_translate_scancode(u8 sc, char& out) 
{
    out = 0;

    // extended prefix пока игнорируем
    if (sc == 0xE0) 
    {
        (void)kbd_read_scancode();
        return false;
    }

    bool released = (sc & 0x80) != 0;
    u8 code = sc & 0x7F;

    switch (code) 
    {
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

    if (is_alpha(c)) 
    {
        bool upper = kbd.shift() ^ kbd.caps_lock;
        if (upper) 
        {
            if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
        } 
        else 
        {
            if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        }
    }

    out = c;
    return true;
}

static void read_line(char* buf, int max_len) {
    int len = 0;

    while (true) 
    {
        char ch;
        if (!kbd_translate_scancode(kbd_read_scancode(), ch)) 
            continue;

        if (ch == '\n') 
        {
            buf[len] = '\0';
            console.putc('\n');
            return;
        }

        if (ch == '\b') 
        {
            if (len > 0) 
            {
                --len;
                console.putc('\b');
            }
            continue;
        }

        if (ch >= 32 && ch <= 126 && len < max_len - 1) 
        {
            buf[len++] = ch;
            console.putc(ch);
        }
    }
}


static void print_banner(unsigned char boot_color) 
{
    console.set_color(boot_color);
    console.println("Welcome to ConvertOS");
    console.set_color(boot_color);
}
static const char* color_name(unsigned char boot_color)
{
    switch(boot_color)
        {
            case 0x07: return "gray";
            case 0x0F: return "white";
            case 0x0E: return "yellow";
            case 0x09: return "blue";
            case 0x0C: return "red";
            case 0x0A: return "green";
            default: return "unknown";
        }
}
static void handle_command(char* line, unsigned char boot_color) {
    const int MAX_ARGS = 4;
    char* argv[MAX_ARGS];
    int argc = parse_args(line, argv, MAX_ARGS);
    if(argc == 0) return;
    const char* cmd = argv[0];

    if (streq(cmd, "")) 
    {
        return;
    }

    if (streq(cmd, "clear")) 
    {
        console.clear();
        return;
    }

    if (streq(cmd, "info")) 
    {
        console.println("ConvertOS:");
        console.println("Developer: Mihajlich Maksim; 5151003/40002");
        console.println("Translator: GNU Assembler; Syntax: AT&T");
        console.print("Current color: ");
        console.print(color_name(boot_color));
        console.print("\n");
        return;
    }
    if (streq(cmd, "shutdown"))
    {
        console.shutdown();
        return;
    }
    if (streq(cmd, "nsconv"))
    {
        char validance = is_valid(argv[1], argv[2]);
        long long line = conv10(argv[1], argv[2]);
        int order = strlen(argv[1]);
        if(argv[1][0] == '-') order--;
        char res[34];
        long long maxint = 2147483647;
        long long minint = -2147483648;
        if (validance != '0')
        {
            console.print("error: invalid symbol: ");
            console.putc(validance);
            console.print("\n");
        }
        else if(order >= 9)
            if(order == 9)
            {
                if(line > maxint || line < minint)
                    console.println("error: int overflow");
            }
            else
                console.println("error: int overflow");
        else if(argc != 4)
            console.println("error: invalid syntax");
        else
        {
            nsconv(line, argv[3], res);
            console.print(res);
            console.print("\n");
        }
        return;
    }
    if(streq(cmd, "posixtime"))
    {
        char res[64];
        get_time(argv[1], 0, res);
        console.println(res);
        return;
    }
    if(streq(cmd, "wintime"))
    {
        char res[64];
        argv[1][strlen(argv[1])-7] = '\0';
        get_time(argv[1], 1, res);
        console.println(res);
        return;
    }
    console.set_color(boot_color);
    console.print("Unknown command: ");
    console.println(cmd);
    console.set_color(boot_color);
}

extern "C" void kmain() 
{
    unsigned char boot_color = load_boot_color();
    console.set_color(boot_color);
    console.clear();

    print_banner(boot_color);

    char line[64];

    while (true) 
    {
        console.set_color(boot_color);
        console.print("> ");
        console.set_color(boot_color);

        read_line(line, sizeof(line));
        handle_command(line, boot_color);
    }
}