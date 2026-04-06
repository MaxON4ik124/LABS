#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
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

typedef int socket_t;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t s32;

static int g_running = 1;

typedef struct Message
{
    u32 idx;
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
    if (b->data)
        free(b->data);
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

static int buf_reserve(Buffer* b, int need)
{
    char* p;
    int new_cap;

    if (need <= b->cap)
        return 1;

    new_cap = (b->cap > 0) ? b->cap : 512;
    while (new_cap < need)
        new_cap *= 2;

    p = (char*)realloc(b->data, (size_t)new_cap);
    if (!p)
        return 0;

    b->data = p;
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

static void message_init(Message* msg)
{
    memset(msg, 0, sizeof(*msg));
}

static void message_free(Message* msg)
{
    if (msg->text)
        free(msg->text);
    msg->text = NULL;
    msg->text_len = 0;
}

static u32 read_u32_be(const unsigned char* p)
{
    return ((u32)p[0] << 24) |
           ((u32)p[1] << 16) |
           ((u32)p[2] << 8) |
           (u32)p[3];
}

static int read_s32_be(const unsigned char* p)
{
    u32 v;
    v = read_u32_be(p);
    return (s32)v;
}

static int shift_msg(Buffer* buf, const char* peer, Message* out_msg)
{
    u32 text_len;
    char* text_copy;
    int total_len;
    FILE* f;

    if (buf->len < 17)
        return 0;

    text_len = read_u32_be((const unsigned char*)(buf->data + 13));
    if (text_len > 1024U * 1024U)
        return -1;

    total_len = 17 + (int)text_len;
    if (buf->len < total_len)
        return 0;

    message_init(out_msg);
    out_msg->idx = read_u32_be((const unsigned char*)(buf->data + 0));
    out_msg->aa = (u16)(((u16)(unsigned char)buf->data[4] << 8) |
                        (u16)(unsigned char)buf->data[5]);
    out_msg->bbb = (s32)read_s32_be((const unsigned char*)(buf->data + 6));
    out_msg->hh = (u8)buf->data[10];
    out_msg->mm = (u8)buf->data[11];
    out_msg->ss = (u8)buf->data[12];
    out_msg->text_len = text_len;

    if (out_msg->hh > 23 || out_msg->mm > 59 || out_msg->ss > 59)
        return -1;

    text_copy = (char*)malloc((size_t)text_len + 1);
    if (!text_copy)
        return -1;

    memcpy(text_copy, buf->data + 17, (size_t)text_len);
    text_copy[text_len] = '\0';
    out_msg->text = text_copy;

    f = fopen("msg.txt", "a");
    if (f)
    {
        fprintf(f, "%s %u %d %02u:%02u:%02u %s\n",
                peer,
                (unsigned int)out_msg->aa,
                (int)out_msg->bbb,
                (unsigned int)out_msg->hh,
                (unsigned int)out_msg->mm,
                (unsigned int)out_msg->ss,
                out_msg->text);
        fclose(f);
    }

    buf_consume(buf, total_len);
    return 1;
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

static int parse_two_digits(const char* p)
{
    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]))
        return -1;
    return (p[0] - '0') * 10 + (p[1] - '0');
}

static int parse_line_to_message(const char* line, Message* out_msg)
{
    const char* p;
    char* endptr;
    unsigned long aa_ul;
    long bbb_l;
    int hh;
    int mm;
    int ss;
    size_t text_len;
    char* text_copy;

    message_init(out_msg);
    p = line;

    if (*p == '\0')
        return -1;

    errno = 0;
    aa_ul = strtoul(p, &endptr, 10);
    if (endptr == p || errno != 0 || aa_ul > 65535UL)
        return -1;
    if (*endptr != ' ')
        return -1;
    p = endptr + 1;

    errno = 0;
    bbb_l = strtol(p, &endptr, 10);
    if (endptr == p || errno != 0)
        return -1;
    if (bbb_l < (-2147483647L - 1L) || bbb_l > 2147483647L)
        return -1;
    if (*endptr != ' ')
        return -1;
    p = endptr + 1;

    if ((int)strlen(p) < 8)
        return -1;

    hh = parse_two_digits(p);
    if (hh < 0 || p[2] != ':')
        return -1;

    mm = parse_two_digits(p + 3);
    if (mm < 0 || p[5] != ':')
        return -1;

    ss = parse_two_digits(p + 6);
    if (ss < 0)
        return -1;

    if (hh > 23 || mm > 59 || ss > 59)
        return -1;

    p += 8;
    if (*p != ' ')
        return -1;
    p++;

    text_len = strlen(p);
    text_copy = (char*)malloc(text_len + 1);
    if (!text_copy)
        return -1;

    memcpy(text_copy, p, text_len + 1);

    out_msg->idx = 0;
    out_msg->aa = (u16)aa_ul;
    out_msg->bbb = (s32)bbb_l;
    out_msg->hh = (u8)hh;
    out_msg->mm = (u8)mm;
    out_msg->ss = (u8)ss;
    out_msg->text = text_copy;
    out_msg->text_len = (u32)text_len;
    return 0;
}

static void write_u32_be(char* p, u32 v)
{
    p[0] = (char)((v >> 24) & 0xFF);
    p[1] = (char)((v >> 16) & 0xFF);
    p[2] = (char)((v >> 8) & 0xFF);
    p[3] = (char)(v & 0xFF);
}

static int send_message(socket_t cn, const Message* msg)
{
    char hdr[17];
    u16 aa_be;
    u32 bbb_be;

    write_u32_be(hdr + 0, msg->idx);
    aa_be = htons(msg->aa);
    bbb_be = htonl((u32)msg->bbb);
    memcpy(hdr + 4, &aa_be, 2);
    memcpy(hdr + 6, &bbb_be, 4);
    hdr[10] = (char)msg->hh;
    hdr[11] = (char)msg->mm;
    hdr[12] = (char)msg->ss;
    write_u32_be(hdr + 13, msg->text_len);

    if (send_all(cn, hdr, (int)sizeof(hdr)) != 0)
        return -1;

    if (msg->text_len > 0)
    {
        if (send_all(cn, msg->text, (int)msg->text_len) != 0)
            return -1;
    }

    return 0;
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
                free(line);
                fclose(f);
                return -1;
            }

            msg.idx = idx;
            if (send_message(cn, &msg) != 0)
            {
                message_free(&msg);
                free(line);
                fclose(f);
                return -1;
            }
            message_free(&msg);
            idx++;
        }

        free(line);
    }

    fclose(f);
    printf("%u messages sent.\n", (unsigned int)idx);
    return 0;
}

static int dispatch(socket_t cn, const char* peer)
{
    Buffer buf;
    char tmp[512];
    char mode[4];
    int mode_set;

    buf_init(&buf);
    mode[0] = '\0';
    mode_set = 0;

    for (;;)
    {
        int n = recv_some(cn, tmp, (int)sizeof(tmp));
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
                int rc;

                message_init(&msg);
                rc = shift_msg(&buf, peer, &msg);
                if (rc < 0)
                {
                    message_free(&msg);
                    buf_free(&buf);
                    return -1;
                }
                if (rc == 0)
                    break;

                if (send_all(cn, "ok", 2) != 0)
                {
                    message_free(&msg);
                    buf_free(&buf);
                    return -1;
                }

                if (strcmp(msg.text, "stop") == 0)
                {
                    printf("'stop' message arrived. Terminating...\n");
                    g_running = 0;
                    message_free(&msg);
                    buf_free(&buf);
                    return 0;
                }

                message_free(&msg);
            }
        }
        else if (mode_set && strcmp(mode, "get") == 0)
        {
            if (send_msgs(cn) != 0)
            {
                buf_free(&buf);
                return -1;
            }
            buf_free(&buf);
            return 0;
        }
    }

    buf_free(&buf);
    return 0;
}

static int server_run(unsigned short port)
{
    socket_t s;
    struct sockaddr_in addr;

    signal(SIGPIPE, SIG_IGN);

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

        snprintf(peer, sizeof(peer), "%s:%u", ipbuf, (unsigned int)ntohs(peer_addr.sin_port));

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

    port = 9000;
    if (argc > 1)
    {
        long p = strtol(argv[1], NULL, 10);
        if (p > 0 && p <= 65535)
            port = (u16)p;
    }

    return server_run(port);
}
