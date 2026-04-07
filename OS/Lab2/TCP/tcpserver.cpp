#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>

typedef int socket_t;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t s32;

enum
{
    READ_CHUNK_SIZE = 512,
    MAX_TEXT_SIZE = 65535
};

typedef struct Message
{
    u16 aa;
    s32 bbb;
    u8 hh;
    u8 mm;
    u8 ss;
    char* text;
    u32 text_len;
} Message;

typedef struct Buffer
{
    char* data;
    int len;
    int cap;
} Buffer;

static int g_running = 1;

static int sock_err(const char* fn)
{
    printf("%s: %s\n", fn, strerror(errno));
    return -1;
}

static void s_close(socket_t s)
{
    close(s);
}

static int send_all(socket_t s, const char* data, int len)
{
    int sent;
    int rc;

    sent = 0;
    while (sent < len)
    {
        rc = (int)send(s, data + sent, (size_t)(len - sent), MSG_NOSIGNAL);
        if (rc <= 0)
            return sock_err("send");
        sent += rc;
    }

    return 0;
}

static int recv_some(socket_t s, char* data, int len)
{
    int rc;

    rc = (int)recv(s, data, (size_t)len, 0);
    if (rc < 0)
        return sock_err("recv");
    return rc;
}

static void buf_init(Buffer* b)
{
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

static void buf_free(Buffer* b)
{
    free(b->data);
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

static int buf_reserve(Buffer* b, int need)
{
    char* tmp;
    int new_cap;

    if (need <= b->cap)
        return 1;

    new_cap = (b->cap == 0) ? 512 : b->cap;
    while (new_cap < need)
        new_cap *= 2;

    tmp = (char*)realloc(b->data, (size_t)new_cap);
    if (!tmp)
        return 0;

    b->data = tmp;
    b->cap = new_cap;
    return 1;
}

static int buf_append(Buffer* b, const char* data, int len)
{
    if (!buf_reserve(b, b->len + len))
        return 0;

    memcpy(b->data + b->len, data, (size_t)len);
    b->len += len;
    return 1;
}

static void buf_consume(Buffer* b, int n)
{
    if (n <= 0)
        return;

    if (n >= b->len)
    {
        b->len = 0;
        return;
    }

    memmove(b->data, b->data + n, (size_t)(b->len - n));
    b->len -= n;
}

static u16 read_u16_be(const unsigned char* p)
{
    return (u16)(((u16)p[0] << 8) | (u16)p[1]);
}

static u32 read_u32_be(const unsigned char* p)
{
    return ((u32)p[0] << 24) |
           ((u32)p[1] << 16) |
           ((u32)p[2] << 8) |
           (u32)p[3];
}

static s32 read_s32_be(const unsigned char* p)
{
    u32 raw;
    s32 out;

    raw = read_u32_be(p);
    memcpy(&out, &raw, sizeof(out));
    return out;
}

static void write_u16_be(char* p, u16 v)
{
    p[0] = (char)((v >> 8) & 0xFF);
    p[1] = (char)(v & 0xFF);
}

static void write_u32_be(char* p, u32 v)
{
    p[0] = (char)((v >> 24) & 0xFF);
    p[1] = (char)((v >> 16) & 0xFF);
    p[2] = (char)((v >> 8) & 0xFF);
    p[3] = (char)(v & 0xFF);
}

static void message_init(Message* msg)
{
    memset(msg, 0, sizeof(*msg));
}

static void message_free(Message* msg)
{
    free(msg->text);
    msg->text = NULL;
    msg->text_len = 0;
}

static int message_set_text(Message* msg, const char* text, u32 len)
{
    char* tmp;

    tmp = (char*)malloc((size_t)len + 1U);
    if (!tmp)
        return -1;

    if (len > 0)
        memcpy(tmp, text, (size_t)len);
    tmp[len] = '\0';

    free(msg->text);
    msg->text = tmp;
    msg->text_len = len;
    return 0;
}

static void msglog(const char* peer, const Message* msg)
{
    FILE* f;

    f = fopen("msg.txt", "ab");
    if (!f)
        return;

    fprintf(f, "%s %u %d %02u:%02u:%02u ",
            peer,
            (unsigned int)msg->aa,
            (int)msg->bbb,
            (unsigned int)msg->hh,
            (unsigned int)msg->mm,
            (unsigned int)msg->ss);
    fwrite(msg->text, 1, (size_t)msg->text_len, f);
    fputc('\n', f);
    fclose(f);
}

static int parse_two_digits(const char* p, u8* out_value)
{
    int value;

    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]))
        return -1;

    value = (p[0] - '0') * 10 + (p[1] - '0');
    *out_value = (u8)value;
    return 0;
}

static int read_line_alloc(FILE* f, char** out_line)
{
    int ch;
    int len;
    int cap;
    char* buf;
    char* tmp;

    *out_line = NULL;
    len = 0;
    cap = 256;
    buf = (char*)malloc((size_t)cap);
    if (!buf)
        return -1;

    for (;;)
    {
        ch = fgetc(f);

        if (ch == EOF)
        {
            if (len == 0)
            {
                free(buf);
                return 0;
            }
            break;
        }

        if (ch == '\n')
            break;

        if (ch == '\r')
        {
            int next = fgetc(f);
            if (next != '\n' && next != EOF)
                ungetc(next, f);
            break;
        }

        if (len + 1 >= cap)
        {
            cap *= 2;
            tmp = (char*)realloc(buf, (size_t)cap);
            if (!tmp)
            {
                free(buf);
                return -1;
            }
            buf = tmp;
        }

        buf[len++] = (char)ch;
    }

    buf[len] = '\0';
    *out_line = buf;
    return 1;
}

static int parse_line_to_message(const char* line, Message* out_msg)
{
    const char* p;
    char* endptr;
    unsigned long long aa_ull;
    long long bbb_ll;
    u8 hh;
    u8 mm;
    u8 ss;
    size_t text_len;

    message_free(out_msg);
    p = line;

    if (*p == '\0')
        return -1;

    errno = 0;
    aa_ull = strtoull(p, &endptr, 10);
    if (endptr == p || errno != 0 || aa_ull > 65535ULL)
        return -1;
    if (*endptr != ' ')
        return -1;
    p = endptr + 1;

    errno = 0;
    bbb_ll = strtoll(p, &endptr, 10);
    if (endptr == p || errno != 0 || bbb_ll < (long long)INT32_MIN || bbb_ll > (long long)INT32_MAX)
        return -1;
    if (*endptr != ' ')
        return -1;
    p = endptr + 1;

    if ((int)strlen(p) < 8)
        return -1;

    if (parse_two_digits(p + 0, &hh) != 0 || p[2] != ':')
        return -1;
    if (parse_two_digits(p + 3, &mm) != 0 || p[5] != ':')
        return -1;
    if (parse_two_digits(p + 6, &ss) != 0)
        return -1;

    if (hh > 23 || mm > 59 || ss > 59)
        return -1;

    p += 8;
    if (*p != ' ')
        return -1;
    p++;

    text_len = strlen(p);

    out_msg->aa = (u16)aa_ull;
    out_msg->bbb = (s32)bbb_ll;
    out_msg->hh = hh;
    out_msg->mm = mm;
    out_msg->ss = ss;

    if (message_set_text(out_msg, p, (u32)text_len) != 0)
        return -1;

    return 0;
}

static int build_message_bytes(u32 idx, const Message* msg, char** out_buf, int* out_len)
{
    char* buf;
    int len;
    u32 bbb_raw;

    len = 4 + 2 + 4 + 3 + 4 + (int)msg->text_len;
    buf = (char*)malloc((size_t)len);
    if (!buf)
        return -1;

    write_u32_be(buf + 0, idx);
    write_u16_be(buf + 4, msg->aa);
    memcpy(&bbb_raw, &msg->bbb, sizeof(bbb_raw));
    write_u32_be(buf + 6, bbb_raw);
    buf[10] = (char)msg->hh;
    buf[11] = (char)msg->mm;
    buf[12] = (char)msg->ss;
    write_u32_be(buf + 13, msg->text_len);
    if (msg->text_len > 0)
        memcpy(buf + 17, msg->text, (size_t)msg->text_len);

    *out_buf = buf;
    *out_len = len;
    return 0;
}

static int shift_msg(Buffer* buf, Message* out_msg, u32* out_idx)
{
    u32 text_len;
    int need;

    if (buf->len < 17)
        return 0;

    text_len = read_u32_be((const unsigned char*)(buf->data + 13));
    if (text_len > MAX_TEXT_SIZE)
        return -1;

    need = 17 + (int)text_len;
    if (buf->len < need)
        return 0;

    message_free(out_msg);
    out_msg->aa = read_u16_be((const unsigned char*)(buf->data + 4));
    out_msg->bbb = read_s32_be((const unsigned char*)(buf->data + 6));
    out_msg->hh = (u8)buf->data[10];
    out_msg->mm = (u8)buf->data[11];
    out_msg->ss = (u8)buf->data[12];

    if (out_msg->hh > 23 || out_msg->mm > 59 || out_msg->ss > 59)
        return -1;

    if (message_set_text(out_msg, buf->data + 17, text_len) != 0)
        return -1;

    *out_idx = read_u32_be((const unsigned char*)(buf->data + 0));
    buf_consume(buf, need);
    return 1;
}

static int send_message(socket_t cn, u32 idx, const Message* msg)
{
    char* buf;
    int len;
    int rc;

    buf = NULL;
    len = 0;

    rc = build_message_bytes(idx, msg, &buf, &len);
    if (rc != 0)
        return -1;

    rc = send_all(cn, buf, len);
    free(buf);
    return rc;
}

static int send_msgs(socket_t cn)
{
    FILE* f;
    char* line;
    u32 idx;

    f = fopen("msg.txt", "rb");
    if (!f)
    {
        printf("0 messages sent.\n");
        return 0;
    }

    idx = 0;
    for (;;)
    {
        int r;
        char* rest;
        Message msg;

        message_init(&msg);
        line = NULL;
        r = read_line_alloc(f, &line);
        if (r < 0)
        {
            fclose(f);
            return -1;
        }
        if (r == 0)
            break;

        if (line[0] == '\0')
        {
            free(line);
            continue;
        }

        rest = strchr(line, ' ');
        if (rest && rest[1] != '\0')
        {
            rest++;
            if (parse_line_to_message(rest, &msg) != 0)
            {
                message_free(&msg);
                free(line);
                fclose(f);
                return -1;
            }

            if (send_message(cn, idx, &msg) != 0)
            {
                message_free(&msg);
                free(line);
                fclose(f);
                return -1;
            }

            idx++;
        }

        message_free(&msg);
        free(line);
    }

    fclose(f);
    printf("%u messages sent.\n", (unsigned int)idx);
    return 0;
}

static int dispatch(socket_t cn, const char* peer)
{
    Buffer buf;
    char tmp[READ_CHUNK_SIZE];
    char mode[4];
    int mode_set;

    buf_init(&buf);
    mode[0] = '\0';
    mode_set = 0;

    for (;;)
    {
        int n;

        n = recv_some(cn, tmp, sizeof(tmp));
        if (n < 0)
        {
            buf_free(&buf);
            return -1;
        }

        if (n == 0)
            break;

        if (!buf_append(&buf, tmp, n))
        {
            buf_free(&buf);
            return -1;
        }

        if (!mode_set && buf.len >= 3)
        {
            memcpy(mode, buf.data, 3);
            mode[3] = '\0';
            buf_consume(&buf, 3);
            mode_set = 1;

            if (strcmp(mode, "put") != 0 && strcmp(mode, "get") != 0)
            {
                buf_free(&buf);
                return -1;
            }
        }

        if (mode_set && strcmp(mode, "put") == 0)
        {
            for (;;)
            {
                Message msg;
                u32 idx;
                int rc;
                int is_stop;

                message_init(&msg);
                idx = 0;
                rc = shift_msg(&buf, &msg, &idx);

                if (rc < 0)
                {
                    message_free(&msg);
                    buf_free(&buf);
                    return -1;
                }

                if (rc == 0)
                {
                    message_free(&msg);
                    break;
                }

                msglog(peer, &msg);
                is_stop = (msg.text && strcmp(msg.text, "stop") == 0);

                if (send_all(cn, "ok", 2) != 0)
                {
                    message_free(&msg);
                    buf_free(&buf);
                    return -1;
                }

                if (is_stop)
                {
                    printf("'stop' message arrived. Terminating...\n");
                    g_running = 0;
                    message_free(&msg);
                    buf_free(&buf);
                    return 0;
                }

                message_free(&msg);
                (void)idx;
            }
        }
        else if (mode_set && strcmp(mode, "get") == 0)
        {
            int rc;
            rc = send_msgs(cn);
            buf_free(&buf);
            return rc;
        }
    }

    buf_free(&buf);
    return 0;
}

static int server_run(unsigned short port)
{
    socket_t s;
    struct sockaddr_in addr;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        return sock_err("socket");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    {
        int opt = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        s_close(s);
        return sock_err("bind");
    }

    if (listen(s, 16) < 0)
    {
        s_close(s);
        return sock_err("listen");
    }

    printf("Listening TCP port: %u\n", (unsigned int)port);

    while (g_running)
    {
        socket_t cn;
        struct sockaddr_in peer_addr;
        socklen_t peer_len;
        char ipbuf[64];
        char peer[128];
        const char* iptxt;

        peer_len = sizeof(peer_addr);
        cn = accept(s, (struct sockaddr*)&peer_addr, &peer_len);
        if (cn < 0)
        {
            if (g_running)
                sock_err("accept");
            break;
        }

        iptxt = inet_ntop(AF_INET, &peer_addr.sin_addr, ipbuf, sizeof(ipbuf));
        if (!iptxt)
            strcpy(ipbuf, "unknown");

        sprintf(peer, "%s:%u", ipbuf, (unsigned int)ntohs(peer_addr.sin_port));
        printf("  Peer connected  : %s\n", peer);

        if (dispatch(cn, peer) != 0)
            printf("  Peer Exception   : %s: 'dispatch failed'\n", peer);

        printf("  Peer disconnected: %s\n", peer);
        s_close(cn);
    }

    s_close(s);
    return 0;
}

int main(int argc, char* argv[])
{
    u16 port;

    signal(SIGPIPE, SIG_IGN);

    port = 9000;
    if (argc > 1)
    {
        long p;
        p = strtol(argv[1], 0, 10);
        if (p > 0 && p <= 65535)
            port = (u16)p;
    }

    return server_run(port);
}
