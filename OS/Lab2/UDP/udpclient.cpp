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
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t s32;

enum
{
    MESSAGE_TEXT_SIZE = 2048,
    MESSAGE_WIRE_SIZE = 2 + 4 + 1 + 1 + 1 + MESSAGE_TEXT_SIZE,
    UDP_PACKET_SIZE = 4 + MESSAGE_WIRE_SIZE
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

typedef struct PendingMessage
{
    u32 idx;
    char* packet;
    int packet_len;
    int confirmed;
} PendingMessage;

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

static u8 parse_two_digits(const char* p)
{
    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]))
        return -1;
    return (p[0] - '0') * 10 + (p[1] - '0');
}

static void message_clear(Message* msg)
{
    memset(msg, 0, sizeof(*msg));
}

static int parse_line(const char* line, Message* out_msg)
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
    out_msg->bbb = (s32)bbb;
    out_msg->hh = (u8)hh;
    out_msg->mm = (u8)mm;
    out_msg->ss = (u8)ss;
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

static void serialize_message(const Message* msg, char out[MESSAGE_WIRE_SIZE])
{
    u16 aa_be;
    s32 bbb_be;

    aa_be = htons(msg->aa);
    bbb_be = htonl(msg->bbb);

    memcpy(out + 0, &aa_be, 2);
    memcpy(out + 2, &bbb_be, 4);
    out[6] = (u8)msg->hh;
    out[7] = (u8)msg->mm;
    out[8] = (u8)msg->ss;
    memcpy(out + 9, msg->message, MESSAGE_TEXT_SIZE);
}

static int build_packet(u32 idx, const Message* msg, char** out_buf, int* out_len)
{
    char* buf;

    buf = (char*)malloc((size_t)UDP_PACKET_SIZE);
    if (!buf)
        return -1;

    write_u32_be(buf + 0, idx);
    serialize_message(msg, buf + 4);

    *out_buf = buf;
    *out_len = UDP_PACKET_SIZE;
    return 0;
}

static int append_message(PendingMessage** messages, int* count, int* cap, PendingMessage* m)
{
    PendingMessage* tmp;
    int new_cap;

    if (*count >= *cap)
    {
        new_cap = (*cap == 0) ? 32 : (*cap * 2);
        tmp = (PendingMessage*)realloc(*messages, (size_t)new_cap * sizeof(PendingMessage));
        if (!tmp)
            return -1;
        *messages = tmp;
        *cap = new_cap;
    }

    (*messages)[*count] = *m;
    (*count)++;
    return 0;
}

static void free_messages(PendingMessage* messages, int count)
{
    int i;

    if (!messages)
        return;

    for (i = 0; i < count; ++i)
        free(messages[i].packet);

    free(messages);
}

static int load_messages_from_file(const char* file_name, PendingMessage** out_messages, int* out_count)
{
    FILE* f;
    PendingMessage* messages;
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
            Message msg;
            char* packet;
            int packet_len;
            PendingMessage m;

            packet = NULL;

            if (parse_line(line, &msg) != 0)
            {
                printf("invalid input line: %s\n", line);
                free(line);
                fclose(f);
                free_messages(messages, count);
                return -1;
            }

            if (build_packet(idx, &msg, &packet, &packet_len) != 0)
            {
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
                free(line);
                fclose(f);
                free_messages(messages, count);
                return -1;
            }

            idx++;
        }

        free(line);
    }

    fclose(f);
    *out_messages = messages;
    *out_count = count;
    return 0;
}

static int send_pending_messages(int sock, PendingMessage* msgs, int total)
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

static void process_ack_datagram(const char* buf, int len, PendingMessage* msgs, int total, int* confirmed_count)
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

static int wait_for_one_ack(int sock, PendingMessage* msgs, int total, int* confirmed_count)
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
    PendingMessage* messages;
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
