#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
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

typedef uint32_t u32;
typedef int32_t s32;
typedef uint16_t u16;

typedef struct Message
{
    u32 idx;
    char* packet;
    int packet_len;
    int confirmed;
} Message;

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
    u32 msg_len_u32;

    total_len = 4 + 2 + 4 + 3 + 4 + msg_len;
    buf = (char*)malloc((size_t)total_len);
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

    msg_len_u32 = (u32)msg_len;
    buf[13] = (char)((msg_len_u32 >> 24) & 0xFF);
    buf[14] = (char)((msg_len_u32 >> 16) & 0xFF);
    buf[15] = (char)((msg_len_u32 >> 8) & 0xFF);
    buf[16] = (char)(msg_len_u32 & 0xFF);

    if (msg_len > 0)
        memcpy(buf + 17, msg, (size_t)msg_len);

    *out_buf = buf;
    *out_len = total_len;
    return 0;
}

static int append_message(Message** messages, int* count, int* cap, Message* m)
{
    Message* tmp;
    int new_cap;

    if (*count >= *cap)
    {
        new_cap = (*cap == 0) ? 32 : (*cap * 2);
        tmp = (Message*)realloc(*messages, (size_t)new_cap * sizeof(Message));
        if (!tmp)
            return -1;
        *messages = tmp;
        *cap = new_cap;
    }

    (*messages)[*count] = *m;
    (*count)++;
    return 0;
}

static void free_messages(Message* messages, int count)
{
    int i;

    if (!messages)
        return;

    for (i = 0; i < count; ++i)
        free(messages[i].packet);

    free(messages);
}

static int load_messages_from_file(const char* file_name, Message** out_messages, int* out_count)
{
    FILE* f;
    Message* messages;
    int count;
    int cap;
    u32 idx;

    f = fopen(file_name, "rb");
    if (!f)
    {
        printf("cannot open file: %s\n", file_name);
        return -1;
    }

    messages = NULL;
    count = 0;
    cap = 0;
    idx = 0;

    for (;;)
    {
        char* line;
        int line_status;

        line = NULL;
        line_status = read_line_alloc(f, &line);
        if (line_status < 0)
        {
            fclose(f);
            free_messages(messages, count);
            return -1;
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
            Message m;

            msg = NULL;
            packet = NULL;

            if (parse_line(line, &aa, &bbb, tm, &msg, &msg_len) != 0)
            {
                printf("invalid input line: %s\n", line);
                free(line);
                fclose(f);
                free_messages(messages, count);
                return -1;
            }

            if (build_packet(idx, aa, bbb, tm, msg, msg_len, &packet, &packet_len) != 0)
            {
                free(msg);
                free(line);
                fclose(f);
                free_messages(messages, count);
                return -1;
            }

            m.idx = idx;
            m.packet = packet;
            m.packet_len = packet_len;
            m.confirmed = 0;

            if (append_message(&messages, &count, &cap, &m) != 0)
            {
                free(packet);
                free(msg);
                free(line);
                fclose(f);
                free_messages(messages, count);
                return -1;
            }

            idx++;
            free(msg);
        }

        free(line);
    }

    fclose(f);
    *out_messages = messages;
    *out_count = count;
    return 0;
}

static int send_pending_messages(int sock, Message* msgs, int total)
{
    int i;

    for (i = 0; i < total; ++i)
    {
        int rc;

        if (msgs[i].confirmed)
            continue;

        rc = (int)send(sock, msgs[i].packet, (size_t)msgs[i].packet_len, 0);
        if (rc < 0)
        {
            printf("send failed: %s\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}

static u32 read_u32_be(const unsigned char* p)
{
    return ((u32)p[0] << 24) |
           ((u32)p[1] << 16) |
           ((u32)p[2] << 8)  |
           (u32)p[3];
}

static void process_ack_datagram(const char* buf, int len, Message* msgs, int total, int* confirmed_count)
{
    int i;

    if (len <= 0)
        return;

    if ((len % 4) != 0)
        return;

    for (i = 0; i < len; i += 4)
    {
        u32 idx;

        idx = read_u32_be((const unsigned char*)(buf + i));
        if (idx < (u32)total)
        {
            if (!msgs[idx].confirmed)
            {
                msgs[idx].confirmed = 1;
                (*confirmed_count)++;
            }
        }
    }
}

static int wait_for_one_ack(int sock, Message* msgs, int total, int* confirmed_count)
{
    fd_set rfds;
    struct timeval tv;
    int sel;
    char ackbuf[2048];
    int n;

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
        return 0;

    n = (int)recv(sock, ackbuf, sizeof(ackbuf), 0);
    if (n < 0)
    {
        printf("recv failed: %s\n", strerror(errno));
        return -1;
    }

    process_ack_datagram(ackbuf, n, msgs, total, confirmed_count);
    return 1;
}

int main(int argc, char* argv[])
{
    const char* endpoint;
    const char* file_name;
    char server_ip[64];
    u16 port;
    int sock;
    struct sockaddr_in addr;
    Message* messages;
    int total_messages;
    int target_count;
    int confirmed_count;
    int rc;

    endpoint = NULL;
    file_name = NULL;
    sock = -1;
    messages = NULL;
    total_messages = 0;
    target_count = 0;
    confirmed_count = 0;
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

    if (load_messages_from_file(file_name, &messages, &total_messages) != 0)
        goto cleanup;

    if (total_messages == 0)
    {
        rc = 0;
        goto cleanup;
    }

    target_count = (total_messages < 20) ? total_messages : 20;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("socket failed: %s\n", strerror(errno));
        goto cleanup;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(server_ip);

    if (addr.sin_addr.s_addr == INADDR_NONE)
    {
        printf("invalid IPv4 address\n");
        goto cleanup;
    }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        printf("connect failed: %s\n", strerror(errno));
        goto cleanup;
    }

    printf("Sending UDP messages to %s:%u\n", server_ip, (unsigned int)port);

    while (confirmed_count < target_count)
    {
        int wait_rc;

        if (send_pending_messages(sock, messages, total_messages) != 0)
            goto cleanup;

        for (;;)
        {
            if (confirmed_count >= target_count)
                break;

            wait_rc = wait_for_one_ack(sock, messages, total_messages, &confirmed_count);
            if (wait_rc < 0)
                goto cleanup;

            if (wait_rc == 0)
                break;
        }
    }

    rc = 0;

cleanup:
    if (sock >= 0)
        close(sock);

    free_messages(messages, total_messages);
    return rc;
}