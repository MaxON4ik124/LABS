#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>

#pragma comment(lib, "ws2_32.lib")

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t s32;

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

static void print_wsa_error_text(const char* where, int err)
{
    char msg[512];
    DWORD n;

    n = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        (DWORD)err,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        msg,
        (DWORD)(sizeof(msg) / sizeof(msg[0])),
        NULL
    );

    if (n == 0)
    {
        printf("%s failed, WSA=%d\n", where, err);
        return;
    }

    while (n > 0 && (msg[n - 1] == '\r' || msg[n - 1] == '\n'))
    {
        msg[n - 1] = '\0';
        n--;
    }

    printf("%s failed: %s\n", where, msg);
}

static int init_winsock(void)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        print_wsa_error_text("WSAStartup", WSAGetLastError());
        return -1;
    }
    return 0;
}

static void deinit_winsock(void)
{
    WSACleanup();
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

static int send_all(SOCKET s, const char* data, int len)
{
    int sent_total;
    int res;

    sent_total = 0;
    while (sent_total < len)
    {
        res = send(s, data + sent_total, len - sent_total, 0);
        if (res == SOCKET_ERROR)
        {
            print_wsa_error_text("send", WSAGetLastError());
            return -1;
        }
        if (res == 0)
        {
            printf("connection closed during send\n");
            return -1;
        }
        sent_total += res;
    }

    return 0;
}

static int recv_all(SOCKET s, char* data, int len)
{
    int got_total;
    int res;

    got_total = 0;
    while (got_total < len)
    {
        res = recv(s, data + got_total, len - got_total, 0);
        if (res == SOCKET_ERROR)
        {
            print_wsa_error_text("recv", WSAGetLastError());
            return -1;
        }
        if (res == 0)
        {
            printf("connection closed during recv\n");
            return -1;
        }
        got_total += res;
    }

    return 0;
}

static int parse_endpoint(const char* endpoint, char* out_ip, size_t out_ip_size, u16* out_port)
{
    const char* colon;
    size_t ip_len;
    char port_buf[16];
    char* endptr;
    unsigned long port_ul;

    colon = strchr(endpoint, ':');
    if (!colon)
        return -1;

    ip_len = (size_t)(colon - endpoint);
    if (ip_len == 0 || ip_len >= out_ip_size)
        return -1;

    memcpy(out_ip, endpoint, ip_len);
    out_ip[ip_len] = '\0';

    if (*(colon + 1) == '\0')
        return -1;

    if (strlen(colon + 1) >= sizeof(port_buf))
        return -1;

    strcpy(port_buf, colon + 1);

    port_ul = strtoul(port_buf, &endptr, 10);
    if (*port_buf == '\0' || *endptr != '\0' || port_ul == 0 || port_ul > 65535UL)
        return -1;

    *out_port = (u16)port_ul;
    return 0;
}

static SOCKET connect_with_retry(const char* ip, u16 port)
{
    SOCKET s;
    struct sockaddr_in addr;
    int attempt;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (addr.sin_addr.s_addr == INADDR_NONE)
    {
        printf("invalid IPv4 address\n");
        return INVALID_SOCKET;
    }

    printf("Connecting to: %s:%u\n", ip, (unsigned int)port);

    for (attempt = 1; attempt <= 10; ++attempt)
    {
        s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET)
        {
            print_wsa_error_text("socket", WSAGetLastError());
            return INVALID_SOCKET;
        }

        if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == 0)
        {
            printf("Connected.\n");
            return s;
        }

        print_wsa_error_text("connect", WSAGetLastError());
        closesocket(s);

        if (attempt < 10)
            Sleep(100);
    }

    printf("connect failed after 10 attempts\n");
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
            ch = fgetc(f);
            if (ch != '\n' && ch != EOF)
                ungetc(ch, f);
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

static int parse_line(const char* line, Message* out_msg)
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

    if ((unsigned char)p[0] == 0xEF &&
        (unsigned char)p[1] == 0xBB &&
        (unsigned char)p[2] == 0xBF)
    {
        p += 3;
    }

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

static int send_message(SOCKET s, u32 idx, const Message* msg)
{
    char hdr[17];
    u16 aa_be;
    u32 bbb_be;

    write_u32_be(hdr + 0, idx);
    aa_be = htons(msg->aa);
    bbb_be = htonl((u32)msg->bbb);
    memcpy(hdr + 4, &aa_be, 2);
    memcpy(hdr + 6, &bbb_be, 4);
    hdr[10] = (char)msg->hh;
    hdr[11] = (char)msg->mm;
    hdr[12] = (char)msg->ss;
    write_u32_be(hdr + 13, msg->text_len);

    if (send_all(s, hdr, (int)sizeof(hdr)) != 0)
        return -1;

    if (msg->text_len > 0)
    {
        if (send_all(s, msg->text, (int)msg->text_len) != 0)
            return -1;
    }

    return 0;
}

static int wait_confirm(SOCKET s)
{
    char buf[2];
    if (recv_all(s, buf, 2) != 0)
        return -1;
    if (buf[0] != 'o' || buf[1] != 'k')
    {
        printf("invalid confirmation\n");
        return -1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    const char* endpoint;
    const char* file_name;
    char server_ip[64];
    u16 port;
    SOCKET sock;
    FILE* f;
    u32 sent_count;
    u32 idx;
    int rc;

    endpoint = NULL;
    file_name = NULL;
    sock = INVALID_SOCKET;
    f = NULL;
    sent_count = 0;
    idx = 0;
    rc = 1;

    if (argc != 3)
    {
        printf("Usage: %s xx.xx.xx.xx:pppp file\n", argv[0]);
        return 1;
    }

    endpoint = argv[1];
    file_name = argv[2];

    if (parse_endpoint(endpoint, server_ip, sizeof(server_ip), &port) != 0)
    {
        printf("invalid endpoint, expected xx.xx.xx.xx:pppp\n");
        return 1;
    }

    if (init_winsock() != 0)
        return 1;

    sock = connect_with_retry(server_ip, port);
    if (sock == INVALID_SOCKET)
        goto cleanup;

    if (send_all(sock, "put", 3) != 0)
        goto cleanup;

    f = fopen(file_name, "rb");
    if (!f)
    {
        printf("cannot open file\n");
        goto cleanup;
    }

    for (;;)
    {
        char* line;
        int line_status;

        line = NULL;
        line_status = read_line_alloc(f, &line);
        if (line_status < 0)
        {
            printf("failed to read file\n");
            goto cleanup;
        }
        if (line_status == 0)
            break;

        if (line[0] != '\0')
        {
            Message msg;

            message_init(&msg);
            if (parse_line(line, &msg) != 0)
            {
                printf("invalid input line\n");
                free(line);
                goto cleanup;
            }

            if (send_message(sock, idx, &msg) != 0)
            {
                message_free(&msg);
                free(line);
                goto cleanup;
            }

            if (wait_confirm(sock) != 0)
            {
                message_free(&msg);
                free(line);
                goto cleanup;
            }

            message_free(&msg);
            sent_count++;
            idx++;
        }

        free(line);
    }

    shutdown(sock, SD_SEND);
    printf("%u message(s) has been sent.\n", (unsigned int)sent_count);
    rc = 0;

cleanup:
    if (f)
        fclose(f);
    if (sock != INVALID_SOCKET)
        closesocket(sock);
    deinit_winsock();
    return rc;
}
