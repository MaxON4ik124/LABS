#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

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

enum
{
    MESSAGE_TEXT_SIZE = 2048,
    MESSAGE_WIRE_SIZE = 2 + 4 + 1 + 1 + 1 + MESSAGE_TEXT_SIZE
};

typedef struct Message
{
    u16 aa;
    s32 bbb;
    u8 hh;
    u8 mm;
    u8 ss;
    char message[MESSAGE_TEXT_SIZE];
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
    int sent = 0;
    int res;

    while (sent < len)
    {
        res = send(s, data + sent, len - sent, MSG_NOSIGNAL);
        if (res <= 0)
            return sock_err("send");
        sent += res;
    }
    return 0;
}

static int recv_some(socket_t s, char* data, int len)
{
    int res = recv(s, data, len, 0);
    if (res < 0)
        return sock_err("recv");
    return res;
}

static void buf_init(Buffer* b)
{
    b->data = 0;
    b->len = 0;
    b->cap = 0;
}

static void buf_free(Buffer* b)
{
    if (b->data)
        free(b->data);
    b->data = 0;
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

    p = (char*)realloc(b->data, new_cap);
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

    memcpy(b->data + b->len, data, len);
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

    memmove(b->data, b->data + n, b->len - n);
    b->len -= n;
}

static void message_clear(Message* msg)
{
    memset(msg, 0, sizeof(*msg));
}

static void deserialize_message(const char* in, Message* out)
{
    u16 aa_be;
    s32 bbb_be;

    message_clear(out);
    memcpy(&aa_be, in + 0, 2);
    memcpy(&bbb_be, in + 2, 4);

    out->aa = ntohs(aa_be);
    out->bbb = ntohl(bbb_be);
    out->hh = (u8)in[6];
    out->mm = (u8)in[7];
    out->ss = (u8)in[8];
    memcpy(out->message, in + 9, MESSAGE_TEXT_SIZE);
    out->message[MESSAGE_TEXT_SIZE - 1] = '\0';
}

static void serialize_message(const Message* msg, char out[MESSAGE_WIRE_SIZE])
{
    u16 aa_be;
    u32 bbb_be;

    aa_be = htons(msg->aa);
    bbb_be = htonl(msg->bbb);

    memcpy(out + 0, &aa_be, 2);
    memcpy(out + 2, &bbb_be, 4);
    out[6] = (u8)msg->hh;
    out[7] = (u8)msg->mm;
    out[8] = (u8)msg->ss;
    memcpy(out + 9, msg->message, MESSAGE_TEXT_SIZE);
}

static void msglog(const char* peer, const Message* msg)
{
    FILE* f = fopen("msg.txt", "a");
    if (!f)
        return;

    fprintf(f, "%s %u %d %02u:%02u:%02u %s\n",
            peer,
            (u16)msg->aa,
            (s32)msg->bbb,
            (u8)msg->hh,
            (u8)msg->mm,
            (u8)msg->ss,
            msg->message);
    fclose(f);
}

static int shift_msg(Buffer* buf, const char* peer, Message* out_msg)
{
    if (buf->len < MESSAGE_WIRE_SIZE)
        return 0;

    deserialize_message(buf->data, out_msg);

    if (out_msg->hh > 23 || out_msg->mm > 59 || out_msg->ss > 59)
        return -1;

    if (strcmp(out_msg->message, "stop") != 0)
        msglog(peer, out_msg);

    buf_consume(buf, MESSAGE_WIRE_SIZE);
    return 1;
}

static int read_line_alloc(FILE* f, char** out_line)
{
    int ch;
    int len;
    int cap;
    char* buf;
    char* tmp;

    *out_line = 0;
    len = 0;
    cap = 256;
    buf = (char*)malloc(cap);
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
            tmp = (char*)realloc(buf, cap);
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

static u8 parse_two_digits(const char* p)
{
    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]))
        return -1;
    return (p[0] - '0') * 10 + (p[1] - '0');
}

static int parse_line_to_message(const char* line, Message* out_msg)
{
    const char* p;
    char* endptr;
    u16 aa;
    s32 bbb;
    u8 hh;
    u8 mm;
    u8 ss;
    size_t msg_len;

    message_clear(out_msg);
    p = line;

    if (*p == '\0')
        return -1;

    errno = 0;
    aa = strtoul(p, &endptr, 10);
    if (endptr == p || errno != 0 || aa > 65535UL)
        return -1;
    if (*endptr != ' ')
        return -1;
    p = endptr + 1;
    errno = 0;
    bbb = strtoul(p, &endptr, 10);
    if (endptr == p || errno != 0 || bbb > 0x7FFFFFFF || bbb < -0x80000000)
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

    if (*p == '\0')
        return -1;

    msg_len = strlen(p);
    if (msg_len >= MESSAGE_TEXT_SIZE)
        return -1;

    out_msg->aa = (u16)aa;
    out_msg->bbb = (u32)bbb;
    out_msg->hh = (u8)hh;
    out_msg->mm = (u8)mm;
    out_msg->ss = (u8)ss;
    memcpy(out_msg->message, p, msg_len + 1);
    return 0;
}

static int send_message(socket_t cn, const Message* msg)
{
    char buf[MESSAGE_WIRE_SIZE];

    serialize_message(msg, buf);
    return send_all(cn, buf, MESSAGE_WIRE_SIZE);
}

static int send_msgs(socket_t cn)
{
    FILE* f;
    char* line;

    f = fopen("msg.txt", "rb");
    if (!f)
    {
        printf("0 messages sent.\n");
        return 0;
    }

    for (;;)
    {
        int r;
        char* rest;
        Message msg;

        line = 0;
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

            if (send_message(cn, &msg) != 0)
            {
                free(line);
                fclose(f);
                return -1;
            }
        }

        free(line);
    }

    fclose(f);
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
        }

        if (mode_set && strcmp(mode, "put") == 0)
        {
            for (;;)
            {
                Message msg;
                int rc;

                rc = shift_msg(&buf, peer, &msg);

                if (rc < 0)
                {
                    buf_free(&buf);
                    return -1;
                }

                if (rc == 0)
                    break;

                if (send_all(cn, "ok", 2) != 0)
                {
                    buf_free(&buf);
                    return -1;
                }

                if (strcmp(msg.message, "stop") == 0)
                {
                    printf("'stop' message arrived. Terminating...\n");
                    buf_free(&buf);
                    g_running = 0;
                    return 0;
                }
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

    port = 9000;

    if (argc > 1)
    {
        long p = strtol(argv[1], 0, 10);
        if (p > 0 && p <= 65535)
            port = (u16)p;
    }

    return server_run(port);
}
