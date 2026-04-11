#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t s32;


typedef SOCKET socket_t;
#define CLOSESOCK closesocket


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

static void message_init(Message* m)
{
    memset(m, 0, sizeof(*m));
}

static void message_free(Message* m)
{
    if (m->message)
    {
        free(m->message);
    }

    message_init(m);
}

static int net_init(void)
{
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;
    return 0;
}

static void net_deinit(void)
{
    WSACleanup();
}

static void msleep_short(int ms)
{
    Sleep((DWORD)ms);
}

static int socket_last_error(void)
{
    return WSAGetLastError();
}


// static int set_socket_timeouts(socket_t s, int ms)
// {
//     DWORD tv;

//     tv = (DWORD)ms;
//     if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) != 0)
//     {
//         return -1;
//     }

//     if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv)) != 0)
//     {
//         return -1;
//     }

//     return 0;
// }

static int send_all(socket_t s, const char* data, int len)
{
    int off;

    off = 0;
    while (off < len)
    {
        int rc;

        rc = send(s, data + off, len - off, 0);

        if (rc <= 0)
        {
            printf("send failed: %d\n", socket_last_error());
            return -1;
        }

        off += rc;
    }

    return 0;
}

static int recv_exact(socket_t s, char* data, int len)
{
    int off;

    off = 0;
    while (off < len)
    {
        int rc;

        rc = recv(s, data + off, len - off, 0);
        if (rc < 0)
        {
            printf("recv failed: %d\n", socket_last_error());
            return -1;
        }

        if (rc == 0)
        {
            printf("connection closed unexpectedly\n");
            return -1;
        }

        off += rc;
    }

    return 0;
}

static int parse_endpoint(const char* endpoint, char* out_ip, size_t out_sz, u16* out_port)
{
    const char* c;
    size_t n;
    char port_buf[16];
    char* endptr;
    unsigned long p;

    c = strchr(endpoint, ':');
    if (!c)
    {
        return -1;
    }

    n = (size_t)(c - endpoint);
    if (n == 0 || n >= out_sz)
    {
        return -1;
    }

    memcpy(out_ip, endpoint, n);
    out_ip[n] = '\0';

    if (*(c + 1) == '\0')
    {
        return -1;
    }

    if (strlen(c + 1) >= sizeof(port_buf))
    {
        return -1;
    }

    strcpy(port_buf, c + 1);

    p = strtoul(port_buf, &endptr, 10);
    if (*port_buf == '\0' || *endptr != '\0' || p == 0 || p > 65535UL)
    {
        return -1;
    }

    *out_port = (u16)p;
    return 0;
}

static socket_t connect_with_retry(const char* ip, u16 port)
{
    struct sockaddr_in addr;
    int i;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (addr.sin_addr.s_addr == INADDR_NONE)
    {
        return INVALID_SOCKET;
    }

    for (i = 0; i < 10; ++i)
    {
        socket_t s;

        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET)
        {
            return INVALID_SOCKET;
        }

        if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == 0)
        {
            return s;
        }

        CLOSESOCK(s);
        msleep_short(100);
    }

    return INVALID_SOCKET;
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
    {
        return -1;
    }

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
        {
            break;
        }

        if (ch == '\r')
        {
            ch = fgetc(f);
            if (ch != '\n' && ch != EOF)
            {
                ungetc(ch, f);
            }

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

static int parse_two_digits(const char* p, u8* out)
{
    int v;

    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]))
    {
        return -1;
    }

    v = (p[0] - '0') * 10 + (p[1] - '0');
    *out = (u8)v;
    return 0;
}

static int parse_line(const char* line, Message* out)
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

    message_free(out);

    p = line;
    if ((unsigned char)p[0] == 0xEF &&
        (unsigned char)p[1] == 0xBB &&
        (unsigned char)p[2] == 0xBF) p += 3;
    if (*p == '\0') return -1;
    errno = 0;
    aa_ul = strtoul(p, &endptr, 10);
    if (endptr == p || errno != 0 || aa_ul > 65535UL) return -1;
    if (*endptr != ' ') return -1;
    p = endptr + 1;
    errno = 0;
    bbb_l = strtol(p, &endptr, 10);
    if (endptr == p || errno != 0 || bbb_l < INT32_MIN || bbb_l > INT32_MAX) return -1;
    if (*endptr != ' ') return -1;
    p = endptr + 1;
    if ((int)strlen(p) < 8) return -1;
    if (parse_two_digits(p, &hh) != 0 || p[2] != ':') return -1;
    if (parse_two_digits(p + 3, &mm) != 0 || p[5] != ':') return -1;
    if (parse_two_digits(p + 6, &ss) != 0) return -1;
    if (hh > 23 || mm > 59 || ss > 59) return -1;
    p += 8;
    if (*p != ' ') return -1;
    ++p;
    if (*p == '\0') return -1;
    
    msg_len = strlen(p);
    text = (char*)malloc(msg_len + 1);
    if (!text) return -1;
    
    memcpy(text, p, msg_len + 1);
    
    out->aa = (u16)aa_ul;
    out->bbb = (s32)bbb_l;
    out->hh = hh;
    out->mm = mm;
    out->ss = ss;
    out->message = text;
    out->message_len = (u32)msg_len;

    return 0;
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

static int send_message(socket_t s, u32 idx, const Message* msg)
{
    int packet_len;
    char* buf;
    u32 bits;
    int rc;

    packet_len = 4 + 2 + 4 + 3 + 4 + (int)msg->message_len;
    buf = (char*)malloc((size_t)packet_len);
    if (!buf)
    {
        return -1;
    }

    write_u32_be(buf + 0, idx);
    write_u16_be(buf + 4, msg->aa);

    memcpy(&bits, &msg->bbb, sizeof(bits));
    write_u32_be(buf + 6, bits);

    buf[10] = (char)msg->hh;
    buf[11] = (char)msg->mm;
    buf[12] = (char)msg->ss;

    write_u32_be(buf + 13, msg->message_len);

    if (msg->message_len > 0)
    {
        memcpy(buf + 17, msg->message, (size_t)msg->message_len);
    }

    rc = send_all(s, buf, packet_len);
    free(buf);

    return rc;
}

static int wait_ok(socket_t s)
{
    char ok[2];

    if (recv_exact(s, ok, 2) != 0)
    {
        return -1;
    }

    if (ok[0] == 'o' && ok[1] == 'k')
    {
        return 0;
    }

    return -1;
}

int main(int argc, char* argv[])
{
    const char* endpoint;
    const char* file_name;
    char ip[64];
    u16 port;
    socket_t sock;
    FILE* f;
    u32 idx;
    int rc;

    sock = INVALID_SOCKET;
    f = NULL;
    idx = 0;
    rc = 1;

    if (argc != 3)
    {
        printf("Usage: %s xx.xx.xx.xx:pppp file\n", argv[0]);
        return 1;
    }

    endpoint = argv[1];
    file_name = argv[2];

    if (parse_endpoint(endpoint, ip, sizeof(ip), &port) != 0)
    {
        return 1;
    }

    if (net_init() != 0)
    {
        return 1;
    }

    sock = connect_with_retry(ip, port);
    if (sock == INVALID_SOCKET)
    {
        printf("connect failed after 10 attempts\n");
        net_deinit();
        return 1;
    }

    // if (set_socket_timeouts(sock, 5000) != 0)
    // {
    //     printf("failed to set socket timeouts: %d\n", socket_last_error());
    //     goto cleanup;
    // }

    if (send_all(sock, "put", 3) != 0)
    {
        goto cleanup;
    }

    f = fopen(file_name, "rb");
    if (!f)
    {
        goto cleanup;
    }

    for (;;)
    {
        char* line;
        int st;
        Message msg;

        line = NULL;
        message_init(&msg);

        st = read_line_alloc(f, &line);
        if (st < 0)
        {
            message_free(&msg);
            goto cleanup;
        }

        if (st == 0)
        {
            message_free(&msg);
            break;
        }

        if (line[0] != '\0')
        {
            if (parse_line(line, &msg) != 0)
            {
                free(line);
                message_free(&msg);
                goto cleanup;
            }

            if (send_message(sock, idx, &msg) != 0)
            {
                free(line);
                message_free(&msg);
                goto cleanup;
            }

            if (wait_ok(sock) != 0)
            {
                free(line);
                message_free(&msg);
                goto cleanup;
            }

            ++idx;
        }

        free(line);
        message_free(&msg);
    }

    printf("%u message(s) has been sent.\n", (unsigned int)idx);
    rc = 0;

cleanup:
    if (f)
    {
        fclose(f);
    }

    if (sock != INVALID_SOCKET)
    {
        CLOSESOCK(sock);
    }

    net_deinit();
    return rc;
}