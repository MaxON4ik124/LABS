#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

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

static int g_running = 1;

typedef struct Message
{
    u16 aa;
    s32 bbb;
    u8 hh;
    u8 mm;
    u8 ss;
    char* message;
    u32 message_len;
} Message;

typedef struct Buffer
{
    char* data;
    int len;
    int cap;
} Buffer;

static void message_init(Message* msg)
{
    msg->aa = 0;
    msg->bbb = 0;
    msg->hh = 0;
    msg->mm = 0;
    msg->ss = 0;
    msg->message = NULL;
    msg->message_len = 0;
}

static void message_free(Message* msg)
{
    if (msg->message)
        free(msg->message);
    message_init(msg);
}

static int sock_err(const char* where)
{
    printf("%s: %s\n", where, strerror(errno));
    return -1;
}

static int send_all(socket_t s, const char* data, int len)
{
    int sent;
    sent = 0;
    while (sent < len)
    {
        int rc = send(s, data + sent, (size_t)(len - sent), MSG_NOSIGNAL);
        if (rc <= 0)
            return sock_err("send");
        sent += rc;
    }
    return 0;
}

static int recv_some(socket_t s, char* data, int len)
{
    int rc = recv(s, data, (size_t)len, 0);
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
    int new_cap;
    char* p;

    if (need <= b->cap)
        return 1;

    new_cap = (b->cap > 0) ? b->cap : 512;
    while (new_cap < need)
    {
        if (new_cap > INT_MAX / 2)
            return 0;
        new_cap *= 2;
    }

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

static int shift_msg(Buffer* buf, u32* out_idx, Message* out_msg)
{
    u32 msg_len;
    u32 bbb_bits;
    int total_len;
    char* text;

    if (buf->len < 17)
        return 0;

    msg_len = read_u32_be((const unsigned char*)(buf->data + 13));
    if (msg_len > (u32)(INT_MAX - 17))
        return -1;

    total_len = 17 + (int)msg_len;
    if (buf->len < total_len)
        return 0;

    if ((u8)buf->data[10] > 23 || (u8)buf->data[11] > 59 || (u8)buf->data[12] > 59)
        return -1;

    text = (char*)malloc((size_t)msg_len + 1);
    if (!text)
        return -1;
    if (msg_len > 0)
        memcpy(text, buf->data + 17, (size_t)msg_len);
    text[msg_len] = '\0';

    message_free(out_msg);
    *out_idx = read_u32_be((const unsigned char*)(buf->data + 0));
    out_msg->aa = read_u16_be((const unsigned char*)(buf->data + 4));
    bbb_bits = read_u32_be((const unsigned char*)(buf->data + 6));
    memcpy(&out_msg->bbb, &bbb_bits, sizeof(out_msg->bbb));
    out_msg->hh = (u8)buf->data[10];
    out_msg->mm = (u8)buf->data[11];
    out_msg->ss = (u8)buf->data[12];
    out_msg->message = text;
    out_msg->message_len = msg_len;

    buf_consume(buf, total_len);
    return 1;
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
    if (msg->message_len > 0)
        fwrite(msg->message, 1, (size_t)msg->message_len, f);
    fputc('\n', f);
    fclose(f);
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

static int parse_two_digits(const char* p, u8* out_value)
{
    int value;
    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]))
        return -1;
    value = (p[0] - '0') * 10 + (p[1] - '0');
    *out_value = (u8)value;
    return 0;
}

static int parse_line_to_message(const char* line, Message* out_msg)
{
    const char* p;
    char* endptr;
    unsigned long aa_ul;
    long bbb_l;
    u8 hh;
    u8 mm;
    u8 ss;
    size_t msg_len;
    char* text;

    message_free(out_msg);
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
    if (endptr == p || errno != 0 || bbb_l < INT32_MIN || bbb_l > INT32_MAX)
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

    msg_len = strlen(p);
    text = (char*)malloc(msg_len + 1);
    if (!text)
        return -1;
    memcpy(text, p, msg_len + 1);

    out_msg->aa = (u16)aa_ul;
    out_msg->bbb = (s32)bbb_l;
    out_msg->hh = hh;
    out_msg->mm = mm;
    out_msg->ss = ss;
    out_msg->message = text;
    out_msg->message_len = (u32)msg_len;
    return 0;
}

static int send_message(socket_t s, u32 idx, const Message* msg)
{
    int packet_len;
    char* buf;
    u32 bbb_bits;
    int rc;

    packet_len = 17 + (int)msg->message_len;
    buf = (char*)malloc((size_t)packet_len);
    if (!buf)
        return -1;

    write_u32_be(buf + 0, idx);
    write_u16_be(buf + 4, msg->aa);
    memcpy(&bbb_bits, &msg->bbb, sizeof(bbb_bits));
    write_u32_be(buf + 6, bbb_bits);
    buf[10] = (char)msg->hh;
    buf[11] = (char)msg->mm;
    buf[12] = (char)msg->ss;
    write_u32_be(buf + 13, msg->message_len);
    if (msg->message_len > 0)
        memcpy(buf + 17, msg->message, (size_t)msg->message_len);

    rc = send_all(s, buf, packet_len);
    free(buf);
    return rc;
}

static int send_msgs(socket_t cn)
{
    FILE* f;
    u32 idx;

    f = fopen("msg.txt", "rb");
    if (!f)
        return 0;

    idx = 0;
    for (;;)
    {
        char* line;
        int st;

        line = NULL;
        st = read_line_alloc(f, &line);
        if (st < 0)
        {
            fclose(f);
            return -1;
        }
        if (st == 0)
            break;

        if (line[0] != '\0')
        {
            char* rest;
            Message msg;
            message_init(&msg);

            rest = strchr(line, ' ');
            if (rest && rest[1] != '\0')
            {
                rest++;
                if (parse_line_to_message(rest, &msg) == 0)
                {
                    if (send_message(cn, idx, &msg) != 0)
                    {
                        free(line);
                        message_free(&msg);
                        fclose(f);
                        return -1;
                    }
                    idx++;
                }
            }
            message_free(&msg);
        }
        free(line);
    }

    fclose(f);
    return 0;
}

static int dispatch(socket_t cn, const char* peer)
{
    Buffer buf;
    char tmp[1024];
    char mode[4];
    int mode_set;

    buf_init(&buf);
    mode[0] = '\0';
    mode_set = 0;

    for (;;)
    {
        int n = recv_some(cn, tmp, sizeof(tmp));
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
            if (strcmp(mode, "get") == 0)
            {
                int rc = send_msgs(cn);
                buf_free(&buf);
                return rc;
            }
        }

        if (mode_set && strcmp(mode, "put") == 0)
        {
            for (;;)
            {
                u32 idx;
                Message msg;
                int st;
                message_init(&msg);
                st = shift_msg(&buf, &idx, &msg);
                if (st < 0)
                {
                    message_free(&msg);
                    buf_free(&buf);
                    return -1;
                }
                if (st == 0)
                {
                    message_free(&msg);
                    break;
                }

                (void)idx;
                msglog(peer, &msg);
                if (send_all(cn, "ok", 2) != 0)
                {
                    message_free(&msg);
                    buf_free(&buf);
                    return -1;
                }

                if (strcmp(msg.message, "stop") == 0)
                {
                    g_running = 0;
                    message_free(&msg);
                    buf_free(&buf);
                    return 0;
                }
                message_free(&msg);
            }
        }
    }

    buf_free(&buf);
    return 0;
}

static int server_run(u16 port)
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
        close(s);
        return sock_err("bind");
    }

    if (listen(s, 16) < 0)
    {
        close(s);
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
        const char* ptxt;

        peer_len = sizeof(peer_addr);
        cn = accept(s, (struct sockaddr*)&peer_addr, &peer_len);
        if (cn < 0)
        {
            if (g_running)
                sock_err("accept");
            break;
        }

        ptxt = inet_ntop(AF_INET, &peer_addr.sin_addr, ipbuf, sizeof(ipbuf));
        if (!ptxt)
            strcpy(ipbuf, "unknown");
        snprintf(peer, sizeof(peer), "%s:%u", ipbuf, (unsigned int)ntohs(peer_addr.sin_port));

        if (dispatch(cn, peer) != 0)
            printf("dispatch failed for %s\n", peer);

        close(cn);
    }

    close(s);
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
