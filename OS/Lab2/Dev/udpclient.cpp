#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
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
    char* message;
    u32 message_len;
} Message;

typedef struct PendingMessage
{
    u32 idx;
    char* packet;
    int packet_len;
    int active;
} PendingMessage;

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
        (unsigned char)p[2] == 0xBF)
    {
        p += 3;
    }

    if (*p == '\0')
    {
        return -1;
    }

    errno = 0;
    aa_ul = strtoul(p, &endptr, 10);
    if (endptr == p || errno != 0 || aa_ul > 65535UL)
    {
        return -1;
    }

    if (*endptr != ' ')
    {
        return -1;
    }

    p = endptr + 1;

    errno = 0;
    bbb_l = strtol(p, &endptr, 10);
    if (endptr == p || errno != 0 || bbb_l < INT32_MIN || bbb_l > INT32_MAX)
    {
        return -1;
    }

    if (*endptr != ' ')
    {
        return -1;
    }

    p = endptr + 1;

    if ((int)strlen(p) < 8)
    {
        return -1;
    }

    if (parse_two_digits(p, &hh) != 0 || p[2] != ':')
    {
        return -1;
    }

    if (parse_two_digits(p + 3, &mm) != 0 || p[5] != ':')
    {
        return -1;
    }

    if (parse_two_digits(p + 6, &ss) != 0)
    {
        return -1;
    }

    if (hh > 23 || mm > 59 || ss > 59)
    {
        return -1;
    }

    p += 8;

    if (*p != ' ')
    {
        return -1;
    }

    ++p;

    if (*p == '\0')
    {
        return -1;
    }

    msg_len = strlen(p);
    text = (char*)malloc(msg_len + 1);
    if (!text)
    {
        return -1;
    }

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

static u32 read_u32_be(const unsigned char* p)
{
    return ((u32)p[0] << 24) |
           ((u32)p[1] << 16) |
           ((u32)p[2] << 8) |
           (u32)p[3];
}

static int build_packet(u32 idx, const Message* msg, char** out_buf, int* out_len)
{
    int packet_len;
    char* buf;
    u32 bits;

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

    *out_buf = buf;
    *out_len = packet_len;
    return 0;
}

static int append_message(PendingMessage** arr, int* count, int* cap, const PendingMessage* m)
{
    PendingMessage* tmp;
    int new_cap;

    if (*count >= *cap)
    {
        new_cap = (*cap == 0) ? 32 : (*cap * 2);
        tmp = (PendingMessage*)realloc(*arr, (size_t)new_cap * sizeof(PendingMessage));
        if (!tmp)
        {
            return -1;
        }

        *arr = tmp;
        *cap = new_cap;
    }

    (*arr)[*count] = *m;
    (*count)++;
    return 0;
}

static void free_messages(PendingMessage* arr, int count)
{
    int i;

    if (!arr)
    {
        return;
    }

    for (i = 0; i < count; ++i)
    {
        free(arr[i].packet);
    }

    free(arr);
}

static int load_messages_from_file(const char* file_name, PendingMessage** out_arr, int* out_count)
{
    FILE* f;
    PendingMessage* arr;
    int count;
    int cap;
    u32 idx;

    f = fopen(file_name, "rb");
    if (!f)
    {
        return -1;
    }

    arr = NULL;
    count = 0;
    cap = 0;
    idx = 0;

    for (;;)
    {
        char* line;
        int st;
        Message msg;
        char* packet;
        int packet_len;
        PendingMessage pm;

        line = NULL;
        packet = NULL;
        message_init(&msg);

        st = read_line_alloc(f, &line);
        if (st < 0)
        {
            fclose(f);
            free_messages(arr, count);
            return -1;
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
                fclose(f);
                free_messages(arr, count);
                return -1;
            }

            if (build_packet(idx, &msg, &packet, &packet_len) != 0)
            {
                free(line);
                message_free(&msg);
                fclose(f);
                free_messages(arr, count);
                return -1;
            }

            pm.idx = idx;
            pm.packet = packet;
            pm.packet_len = packet_len;
            pm.active = 1;

            if (append_message(&arr, &count, &cap, &pm) != 0)
            {
                free(packet);
                free(line);
                message_free(&msg);
                fclose(f);
                free_messages(arr, count);
                return -1;
            }

            ++idx;
        }

        free(line);
        message_free(&msg);
    }

    fclose(f);
    *out_arr = arr;
    *out_count = count;
    return 0;
}

static int queue_count(PendingMessage* arr, int total)
{
    int i;
    int n;

    n = 0;
    for (i = 0; i < total; ++i)
    {
        if (arr[i].active)
        {
            ++n;
        }
    }

    return n;
}

static int send_next_ten(int sock, const struct sockaddr_in* addr, PendingMessage* arr, int total)
{
    int i;
    int cnt;

    cnt = 0;
    for (i = 0; i < total; ++i)
    {
        int rc;

        if (!arr[i].active)
        {
            continue;
        }

        rc = (int)sendto(
            sock,
            arr[i].packet,
            (size_t)arr[i].packet_len,
            0,
            (const struct sockaddr*)addr,
            sizeof(*addr)
        );

        if (rc < 0)
        {
            printf("sendto failed: %s\n", strerror(errno));
            return -1;
        }

        ++cnt;
        if (cnt >= 10)
        {
            break;
        }
    }

    return 0;
}

static void apply_ack_datagram(const char* buf, int len, PendingMessage* arr, int total)
{
    int i;

    if (len <= 0 || (len % 4) != 0)
    {
        return;
    }

    for (i = 0; i + 4 <= len; i += 4)
    {
        u32 idx;

        idx = read_u32_be((const unsigned char*)(buf + i));
        if (idx < (u32)total)
        {
            arr[idx].active = 0;
        }
    }
}

static int drain_acks(int sock, PendingMessage* arr, int total)
{
    char ackbuf[65535];

    for (;;)
    {
        fd_set rfds;
        struct timeval tv;
        int sel;

        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        sel = select(sock + 1, &rfds, NULL, NULL, &tv);
        if (sel < 0)
        {
            printf("select failed: %s\n", strerror(errno));
            return -1;
        }

        if (sel == 0)
        {
            break;
        }

        if (FD_ISSET(sock, &rfds))
        {
            int n;
            struct sockaddr_in from;
            socklen_t from_len;

            memset(&from, 0, sizeof(from));
            from_len = sizeof(from);

            n = (int)recvfrom(
                sock,
                ackbuf,
                sizeof(ackbuf),
                0,
                (struct sockaddr*)&from,
                &from_len
            );

            if (n < 0)
            {
                printf("recvfrom failed: %s\n", strerror(errno));
                return -1;
            }

            apply_ack_datagram(ackbuf, n, arr, total);
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    const char* endpoint;
    const char* file_name;
    char ip[64];
    u16 port;
    int sock;
    struct sockaddr_in addr;
    PendingMessage* msgs;
    int total;
    int total_cnt;
    int rc;

    sock = -1;
    msgs = NULL;
    total = 0;
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

    if (load_messages_from_file(file_name, &msgs, &total) != 0)
    {
        goto cleanup;
    }

    if (total == 0)
    {
        rc = 0;
        goto cleanup;
    }

    total_cnt = total;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        goto cleanup;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1)
    {
        goto cleanup;
    }

    for (;;)
    {
        int cur_cnt;

        cur_cnt = queue_count(msgs, total);
        if (total_cnt - cur_cnt >= 20 || cur_cnt == 0)
        {
            break;
        }

        if (send_next_ten(sock, &addr, msgs, total) != 0)
        {
            goto cleanup;
        }

        if (drain_acks(sock, msgs, total) != 0)
        {
            goto cleanup;
        }
    }

    printf("%d message(s) reached the server.\n", total_cnt - queue_count(msgs, total));
    rc = 0;

cleanup:
    if (sock >= 0)
    {
        close(sock);
    }

    free_messages(msgs, total);
    return rc;
}