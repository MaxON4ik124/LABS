#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t s32;

#define MESSAGE_TEXT_SIZE 65535

#ifdef _WIN32
typedef SOCKET socket_t;
#define INVALID_SOCK INVALID_SOCKET
#else
typedef int socket_t;
#define INVALID_SOCK (-1)
#endif

typedef struct Message
{
    u16 aa;
    s32 bbb;
    u8 hh;
    u8 mm;
    u8 ss;
    char message[MESSAGE_TEXT_SIZE];
} Message;

static void net_sleep_ms(int ms)
{
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    usleep((useconds_t)ms * 1000U);
#endif
}

static void socket_close(socket_t s)
{
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

static int net_init(void)
{
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed, WSA=%d\n", WSAGetLastError());
        return -1;
    }
#endif
    return 0;
}

static void net_deinit(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

static void print_sock_error(const char* where)
{
#ifdef _WIN32
    printf("%s failed, WSA=%d\n", where, WSAGetLastError());
#else
    printf("%s failed: %s\n", where, strerror(errno));
#endif
}

static int send_all(socket_t s, const char* data, int len)
{
    int sent_total;
    int res;

    sent_total = 0;
    while (sent_total < len)
    {
#ifdef _WIN32
        res = send(s, data + sent_total, len - sent_total, 0);
#else
        res = (int)send(s, data + sent_total, (size_t)(len - sent_total), 0);
#endif
        if (res < 0)
        {
            print_sock_error("send");
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

static int recv_all(socket_t s, char* data, int len)
{
    int got_total;
    int res;

    got_total = 0;
    while (got_total < len)
    {
#ifdef _WIN32
        res = recv(s, data + got_total, len - got_total, 0);
#else
        res = (int)recv(s, data + got_total, (size_t)(len - got_total), 0);
#endif
        if (res < 0)
        {
            print_sock_error("recv");
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

static int wait_ok(socket_t s)
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
    if (*port_buf == '\0' || *endptr != '\0' || port_ul > 65535UL)
        return -1;

    *out_port = (u16)port_ul;
    return 0;
}

static int ipv4_to_addr(const char* ip, struct sockaddr_in* out_addr)
{
    unsigned long a;

    memset(out_addr, 0, sizeof(*out_addr));
    out_addr->sin_family = AF_INET;

    a = inet_addr(ip);
    if (a == INADDR_NONE && strcmp(ip, "255.255.255.255") != 0)
        return -1;

    out_addr->sin_addr.s_addr = a;
    return 0;
}

static socket_t connect_with_retry(const char* ip, u16 port)
{
    socket_t s;
    struct sockaddr_in addr;
    int attempt;

    if (ipv4_to_addr(ip, &addr) != 0)
    {
        printf("invalid IPv4 address\n");
        return INVALID_SOCK;
    }

    addr.sin_port = htons(port);

    printf("Connecting to: %s:%u\n", ip, (unsigned int)port);

    for (attempt = 1; attempt <= 10; ++attempt)
    {
        s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCK)
        {
            print_sock_error("socket");
            return INVALID_SOCK;
        }

        if (connect(s, (struct sockaddr*)&addr, (int)sizeof(addr)) == 0)
        {
            printf("Connected.\n");
            return s;
        }

        print_sock_error("connect");
        socket_close(s);

        if (attempt < 10)
            net_sleep_ms(100);
    }

    printf("Failed connect\n");
    return INVALID_SOCK;
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

static void message_clear(Message* msg)
{
    memset(msg, 0, sizeof(*msg));
}

static int parse_two_digits(const char* p, u8* out)
{
    int value;

    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]))
        return -1;

    value = (p[0] - '0') * 10 + (p[1] - '0');
    *out = (u8)value;
    return 0;
}

static int parse_line(const char* line, Message* out_msg)
{
    const char* p;
    char* endptr;
    unsigned long aa_ul;
    long bbb_l;
    u8 hh;
    u8 mm;
    u8 ss;
    size_t msg_len;

    message_clear(out_msg);
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
    if (endptr == p || errno != 0 || bbb_l < (-2147483647L - 1L) || bbb_l > 2147483647L)
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

    if (*p == '\0')
        return -1;

    msg_len = strlen(p);
    if (msg_len >= MESSAGE_TEXT_SIZE)
        return -1;

    out_msg->aa = (u16)aa_ul;
    out_msg->bbb = (s32)bbb_l;
    out_msg->hh = hh;
    out_msg->mm = mm;
    out_msg->ss = ss;
    memcpy(out_msg->message, p, msg_len + 1);

    return 0;
}

static void write_u32_be(char* p, u32 v)
{
    p[0] = (char)((v >> 24) & 0xFF);
    p[1] = (char)((v >> 16) & 0xFF);
    p[2] = (char)((v >> 8) & 0xFF);
    p[3] = (char)(v & 0xFF);
}

static int build_packet(u32 idx, const Message* msg, char** out_buf, int* out_len)
{
    size_t msg_len;
    int packet_len;
    char* buf;
    u16 aa_be;
    u32 bbb_bits;
    u32 msg_len_u32;

    msg_len = strlen(msg->message);
    if (msg_len > 0xFFFFFFFFu)
        return -1;

    packet_len = 4 + 2 + 4 + 3 + 4 + (int)msg_len;
    buf = (char*)malloc((size_t)packet_len);
    if (!buf)
        return -1;

    write_u32_be(buf + 0, idx);
    aa_be = htons(msg->aa);
    memcpy(buf + 4, &aa_be, 2);

    bbb_bits = (u32)msg->bbb;
    write_u32_be(buf + 6, bbb_bits);

    buf[10] = (char)msg->hh;
    buf[11] = (char)msg->mm;
    buf[12] = (char)msg->ss;

    msg_len_u32 = (u32)msg_len;
    write_u32_be(buf + 13, msg_len_u32);
    if (msg_len > 0)
        memcpy(buf + 17, msg->message, msg_len);

    *out_buf = buf;
    *out_len = packet_len;
    return 0;
}

static int send_message(socket_t s, u32 idx, const Message* msg)
{
    char* packet;
    int packet_len;
    int rc;

    packet = NULL;
    packet_len = 0;

    if (build_packet(idx, msg, &packet, &packet_len) != 0)
    {
        printf("failed to build packet\n");
        return -1;
    }

    rc = send_all(s, packet, packet_len);
    free(packet);
    return rc;
}

int main(int argc, char* argv[])
{
    const char* endpoint;
    const char* file_name;
    char server_ip[64];
    u16 port;
    socket_t sock;
    FILE* f;
    u32 sent_count;
    int rc;

    endpoint = NULL;
    file_name = NULL;
    sock = INVALID_SOCK;
    f = NULL;
    sent_count = 0;
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

    if (net_init() != 0)
        return 1;

    sock = connect_with_retry(server_ip, port);
    if (sock == INVALID_SOCK)
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

            if (parse_line(line, &msg) != 0)
            {
                printf("invalid input line\n");
                free(line);
                goto cleanup;
            }

            if (send_message(sock, sent_count, &msg) != 0)
            {
                free(line);
                goto cleanup;
            }

            if (wait_ok(sock) != 0)
            {
                free(line);
                goto cleanup;
            }

            sent_count++;
        }

        free(line);
    }

    rc = 0;
    printf("%u message(s) has been sent.\n", (unsigned int)sent_count);

cleanup:
    if (f)
        fclose(f);
    if (sock != INVALID_SOCK)
        socket_close(sock);
    net_deinit();
    return rc;
}
