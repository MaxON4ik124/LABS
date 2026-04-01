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

static int g_running = 1;

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

static uint16_t get_u16_be(const unsigned char* p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static uint32_t get_u32_be(const unsigned char* p)
{
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |
           (uint32_t)p[3];
}

static int32_t get_s32_be(const unsigned char* p)
{
    uint32_t u = get_u32_be(p);
    return (int32_t)u;
}

static void put_u16_be(char* p, uint16_t v)
{
    p[0] = (char)((v >> 8) & 0xFF);
    p[1] = (char)(v & 0xFF);
}

static void put_u32_be(char* p, uint32_t v)
{
    p[0] = (char)((v >> 24) & 0xFF);
    p[1] = (char)((v >> 16) & 0xFF);
    p[2] = (char)((v >> 8) & 0xFF);
    p[3] = (char)(v & 0xFF);
}

static void msglog(const char* peer, uint16_t aa, int32_t bbb,
                   unsigned char hh, unsigned char mm, unsigned char ss,
                   const char* text)
{
    FILE* f = fopen("msg.txt", "a");
    if (!f)
        return;

    fprintf(f, "%s %u %d %02u:%02u:%02u %s\n",
            peer,
            (unsigned int)aa,
            (int)bbb,
            (unsigned int)hh,
            (unsigned int)mm,
            (unsigned int)ss,
            text ? text : "");
    fclose(f);
}

static int shift_msg(Buffer* buf, const char* peer, char** out_text)
{
    int pos;
    uint16_t aa;
    int32_t bbb;
    unsigned char hh, mm, ss;
    uint32_t msg_len;
    char* text;

    *out_text = 0;
    pos = 0;

    if (buf->len < 4)
        return 0;

    pos += 4;

    if (buf->len - pos < 2)
        return 0;
    aa = get_u16_be((const unsigned char*)(buf->data + pos));
    pos += 2;

    if (buf->len - pos < 4)
        return 0;
    bbb = get_s32_be((const unsigned char*)(buf->data + pos));
    pos += 4;

    if (buf->len - pos < 3)
        return 0;
    hh = (unsigned char)buf->data[pos + 0];
    mm = (unsigned char)buf->data[pos + 1];
    ss = (unsigned char)buf->data[pos + 2];
    pos += 3;

    if (buf->len - pos < 4)
        return 0;
    msg_len = get_u32_be((const unsigned char*)(buf->data + pos));
    pos += 4;

    if ((uint32_t)(buf->len - pos) < msg_len)
        return 0;

    text = (char*)malloc((size_t)msg_len + 1);
    if (!text)
        return -1;

    if (msg_len > 0)
        memcpy(text, buf->data + pos, msg_len);
    text[msg_len] = '\0';
    pos += (int)msg_len;

    msglog(peer, aa, bbb, hh, mm, ss, text);
    buf_consume(buf, pos);

    *out_text = text;
    return 1;
}

static int parse_uint_token(const char** ps, unsigned long* out)
{
    char* endptr;
    unsigned long v;

    if (!isdigit((unsigned char)(*ps)[0]))
        return 0;

    v = strtoul(*ps, &endptr, 10);
    if (endptr == *ps)
        return 0;

    *out = v;
    *ps = endptr;
    return 1;
}

static int parse_sint_token(const char** ps, long* out)
{
    char* endptr;
    long v;

    if ((*ps)[0] != '-' && !isdigit((unsigned char)(*ps)[0]))
        return 0;

    v = strtol(*ps, &endptr, 10);
    if (endptr == *ps)
        return 0;

    *out = v;
    *ps = endptr;
    return 1;
}

static int eat_space(const char** ps)
{
    if ((*ps)[0] != ' ')
        return 0;
    (*ps)++;
    return 1;
}

static int parse_time_token(const char** ps,
                            unsigned char* hh,
                            unsigned char* mm,
                            unsigned char* ss)
{
    const char* p = *ps;
    int h, m, s;

    if (!(isdigit((unsigned char)p[0]) && isdigit((unsigned char)p[1]) &&
          p[2] == ':' &&
          isdigit((unsigned char)p[3]) && isdigit((unsigned char)p[4]) &&
          p[5] == ':' &&
          isdigit((unsigned char)p[6]) && isdigit((unsigned char)p[7])))
    {
        return 0;
    }

    h = (p[0] - '0') * 10 + (p[1] - '0');
    m = (p[3] - '0') * 10 + (p[4] - '0');
    s = (p[6] - '0') * 10 + (p[7] - '0');

    *hh = (unsigned char)h;
    *mm = (unsigned char)m;
    *ss = (unsigned char)s;

    *ps += 8;
    return 1;
}

static int build_msg_packet(uint32_t idx, const char* str, char** out_buf, int* out_len)
{
    const char* p;
    unsigned long aa_ul;
    long bbb_l;
    uint16_t aa;
    int32_t bbb;
    unsigned char hh, mm, ss;
    uint32_t msg_len;
    int total_len;
    char* buf;

    p = str;
    *out_buf = 0;
    *out_len = 0;

    if (!parse_uint_token(&p, &aa_ul))
        return 0;
    if (!eat_space(&p))
        return 0;

    if (!parse_sint_token(&p, &bbb_l))
        return 0;
    if (!eat_space(&p))
        return 0;

    if (!parse_time_token(&p, &hh, &mm, &ss))
        return 0;
    if (!eat_space(&p))
        return 0;

    aa = (uint16_t)(aa_ul & 0xFFFFUL);
    bbb = (int32_t)bbb_l;
    msg_len = (uint32_t)strlen(p);

    total_len = 4 + 2 + 4 + 3 + 4 + (int)msg_len;
    buf = (char*)malloc((size_t)total_len);
    if (!buf)
        return -1;

    put_u32_be(buf + 0, idx);
    put_u16_be(buf + 4, aa);
    put_u32_be(buf + 6, (uint32_t)bbb);
    buf[10] = (char)hh;
    buf[11] = (char)mm;
    buf[12] = (char)ss;
    put_u32_be(buf + 13, msg_len);

    if (msg_len > 0)
        memcpy(buf + 17, p, msg_len);

    *out_buf = buf;
    *out_len = total_len;
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

static int send_msg(socket_t cn, uint32_t idx, const char* str)
{
    char* buf;
    int len;
    int rc;

    rc = build_msg_packet(idx, str, &buf, &len);
    if (rc <= 0)
        return rc;

    rc = send_all(cn, buf, len);
    free(buf);

    if (rc != 0)
        return -1;

    return 1;
}

static int send_msgs(socket_t cn)
{
    FILE* f;
    char* line;
    uint32_t idx;

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
            if (send_msg(cn, idx, rest) <= 0)
            {
                free(line);
                fclose(f);
                return -1;
            }
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
                char* text;
                int rc;

                text = 0;
                rc = shift_msg(&buf, peer, &text);

                if (rc < 0)
                {
                    buf_free(&buf);
                    return -1;
                }

                if (rc == 0)
                    break;

                if (send_all(cn, "ok", 2) != 0)
                {
                    free(text);
                    buf_free(&buf);
                    return -1;
                }

                if (strcmp(text, "stop") == 0)
                {
                    printf("'stop' message arrived. Terminating...\n");
                    free(text);
                    buf_free(&buf);
                    g_running = 0;
                    return 0;
                }

                free(text);
            }
        }
        else if (mode_set && strcmp(mode, "get") == 0)
        {
            send_msgs(cn);
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
    unsigned short port;

    port = 9000;

    if (argc > 1)
    {
        long p = strtol(argv[1], 0, 10);
        if (p > 0 && p <= 65535)
            port = (unsigned short)p;
    }

    return server_run(port);
}