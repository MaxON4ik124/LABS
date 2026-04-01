#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#pragma comment(lib, "ws2_32.lib")

typedef unsigned __int32 u32;
typedef __int32 s32;
typedef unsigned short u16;

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
    u_long mode;

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

        mode = 0;
        if (ioctlsocket(s, FIONBIO, &mode) != 0)
        {
            print_wsa_error_text("ioctlsocket", WSAGetLastError());
            closesocket(s);
            return INVALID_SOCKET;
        }

        if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == 0)
        {
            printf("successfully connected to: %s:%u\n", ip, (unsigned int)port);
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
            ch = fgetc(f);
            if (ch != '\n' && ch != EOF)
                ungetc(ch, f);
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

static int parse_two_digits(const char* p)
{
    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]))
        return -1;
    return (p[0] - '0') * 10 + (p[1] - '0');
}

static int parse_line(
    const char* line,
    u16* out_aa,
    s32* out_bbb,
    unsigned char out_time[3],
    char** out_msg,
    int* out_msg_len)
{
    const char* p;
    char* endptr;
    unsigned long aa;
    long bbb;
    int hh;
    int mm;
    int ss;
    int msg_len;
    char* msg;

    *out_msg = NULL;
    *out_msg_len = 0;

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
    aa = strtoul(p, &endptr, 10);
    if (endptr == p || errno != 0 || aa > 65535UL)
        return -1;
    if (*endptr != ' ')
        return -1;
    p = endptr + 1;

    errno = 0;
    bbb = strtol(p, &endptr, 10);
    if (endptr == p || errno != 0)
        return -1;
    if (bbb < (-2147483647L - 1L) || bbb > 2147483647L)
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

    msg_len = (int)strlen(p);
    msg = (char*)malloc((size_t)msg_len + 1);
    if (!msg)
        return -1;

    memcpy(msg, p, (size_t)msg_len + 1);

    *out_aa = (u16)aa;
    *out_bbb = (s32)bbb;
    out_time[0] = (unsigned char)hh;
    out_time[1] = (unsigned char)mm;
    out_time[2] = (unsigned char)ss;
    *out_msg = msg;
    *out_msg_len = msg_len;

    return 0;
}

static int build_packet(
    u32 idx,
    u16 aa,
    s32 bbb,
    const unsigned char tm[3],
    const char* msg,
    int msg_len,
    char** out_buf,
    int* out_len)
{
    int total_len;
    char* buf;
    u32 bbb_bits;

    total_len = 4 + 2 + 4 + 3 + 4 + msg_len;
    buf = (char*)malloc(total_len);
    if (!buf)
        return -1;

    buf[0] = (char)((idx >> 24) & 0xFF);
    buf[1] = (char)((idx >> 16) & 0xFF);
    buf[2] = (char)((idx >> 8) & 0xFF);
    buf[3] = (char)(idx & 0xFF);

    buf[4] = (char)((aa >> 8) & 0xFF);
    buf[5] = (char)(aa & 0xFF);

    bbb_bits = (u32)bbb;
    buf[6] = (char)((bbb_bits >> 24) & 0xFF);
    buf[7] = (char)((bbb_bits >> 16) & 0xFF);
    buf[8] = (char)((bbb_bits >> 8) & 0xFF);
    buf[9] = (char)(bbb_bits & 0xFF);

    buf[10] = (char)tm[0];
    buf[11] = (char)tm[1];
    buf[12] = (char)tm[2];

    buf[13] = (char)(((u32)msg_len >> 24) & 0xFF);
    buf[14] = (char)(((u32)msg_len >> 16) & 0xFF);
    buf[15] = (char)(((u32)msg_len >> 8) & 0xFF);
    buf[16] = (char)((u32)msg_len & 0xFF);

    if (msg_len > 0)
        memcpy(buf + 17, msg, msg_len);

    *out_buf = buf;
    *out_len = total_len;
    return 0;
}

static int recv_all_replies(SOCKET s, u32 expected_ok_count, int* out_stop_received)
{
    char recvbuf[512];
    char pending[2048];
    int pending_len;
    u32 ok_count;
    int n;

    *out_stop_received = 0;
    pending_len = 0;
    ok_count = 0;

    while (ok_count < expected_ok_count)
    {
        n = recv(s, recvbuf, sizeof(recvbuf), 0);
        if (n == SOCKET_ERROR)
        {
            print_wsa_error_text("recv", WSAGetLastError());
            return -1;
        }

        if (n == 0)
        {
            printf("server closed connection before all ok were received (got %u of %u)\n",
                   (unsigned int)ok_count, (unsigned int)expected_ok_count);
            return -1;
        }

        if (pending_len + n > (int)sizeof(pending))
        {
            printf("response buffer overflow\n");
            return -1;
        }

        memcpy(pending + pending_len, recvbuf, n);
        pending_len += n;

        for (;;)
        {
            if (pending_len >= 2 && pending[0] == 'o' && pending[1] == 'k')
            {
                memmove(pending, pending + 2, pending_len - 2);
                pending_len -= 2;
                ok_count++;
                continue;
            }

            if (pending_len >= 4 &&
                pending[0] == 's' &&
                pending[1] == 't' &&
                pending[2] == 'o' &&
                pending[3] == 'p')
            {
                *out_stop_received = 1;
                return 0;
            }

            if (pending_len > 0)
            {
                if (pending[0] == 'o' && pending_len < 2)
                    break;

                if (pending[0] == 's' && pending_len < 4)
                    break;

                printf("invalid server response\n");
                return -1;
            }

            break;
        }
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
    int stop_received;
    int rc;

    endpoint = NULL;
    file_name = NULL;
    sock = INVALID_SOCKET;
    f = NULL;
    sent_count = 0;
    stop_received = 0;
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
            u16 aa;
            s32 bbb;
            unsigned char tm[3];
            char* msg;
            int msg_len;
            char* packet;
            int packet_len;

            msg = NULL;
            packet = NULL;

            if (parse_line(line, &aa, &bbb, tm, &msg, &msg_len) != 0)
            {
                printf("invalid input line\n");
                free(line);
                goto cleanup;
            }

            if (build_packet(sent_count, aa, bbb, tm, msg, msg_len, &packet, &packet_len) != 0)
            {
                free(msg);
                free(line);
                printf("failed to build packet\n");
                goto cleanup;
            }

            if (send_all(sock, packet, packet_len) != 0)
            {
                free(packet);
                free(msg);
                free(line);
                goto cleanup;
            }

            sent_count++;

            free(packet);
            free(msg);
        }

        free(line);
    }

    if (recv_all_replies(sock, sent_count, &stop_received) != 0)
        goto cleanup;

    if (stop_received)
    {
        closesocket(sock);
        sock = INVALID_SOCKET;
        rc = 0;
        goto cleanup;
    }
    
    rc = 0;
    printf("%d message(s) has been sent.", sent_count);

cleanup:
    if (f)
        fclose(f);
    if (sock != INVALID_SOCKET)
        closesocket(sock);
    deinit_winsock();
    return rc;
}