#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

typedef unsigned __int32 u32;
typedef __int32 s32;
typedef unsigned short u16;

#define MAX_RECENT_ACK 20
#define CLIENT_TTL_SEC 30

typedef struct ParsedMessage
{
    u32 idx;
    u16 aa;
    s32 bbb;
    unsigned char hh;
    unsigned char mm;
    unsigned char ss;
    const char* text_ptr;
    u32 text_len;
    int is_stop;
} ParsedMessage;

typedef struct ClientInfo
{
    struct sockaddr_in addr;
    time_t last_seen;

    u32* seen_ids;
    int seen_count;
    int seen_cap;

    u32 recent_ids[MAX_RECENT_ACK];
    int recent_count;
} ClientInfo;

static ClientInfo* g_clients = NULL;
static int g_client_count = 0;
static int g_client_cap = 0;
static int g_running = 1;

static void print_wsa_error(const char* where)
{
    printf("%s failed, WSA=%d\n", where, WSAGetLastError());
}

static int init_winsock(void)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        print_wsa_error("WSAStartup");
        return -1;
    }
    return 0;
}

static void deinit_winsock(void)
{
    WSACleanup();
}

static u16 read_u16_be(const unsigned char* p)
{
    return (u16)(((u16)p[0] << 8) | (u16)p[1]);
}

static u32 read_u32_be(const unsigned char* p)
{
    return ((u32)p[0] << 24) |
           ((u32)p[1] << 16) |
           ((u32)p[2] << 8)  |
           (u32)p[3];
}

static s32 read_s32_be(const unsigned char* p)
{
    return (s32)read_u32_be(p);
}

static void write_u32_be(char* p, u32 v)
{
    p[0] = (char)((v >> 24) & 0xFF);
    p[1] = (char)((v >> 16) & 0xFF);
    p[2] = (char)((v >> 8) & 0xFF);
    p[3] = (char)(v & 0xFF);
}

static void peer_to_string(const struct sockaddr_in* addr, char* out, int out_size)
{
    const unsigned char* ip;

    ip = (const unsigned char*)&addr->sin_addr.s_addr;
    _snprintf(out, out_size, "%u.%u.%u.%u:%u",
              (unsigned int)ip[0],
              (unsigned int)ip[1],
              (unsigned int)ip[2],
              (unsigned int)ip[3],
              (unsigned int)ntohs(addr->sin_port));
    out[out_size - 1] = '\0';
}

static int same_client_id(const struct sockaddr_in* a, const struct sockaddr_in* b)
{
    if (a->sin_family != b->sin_family)
        return 0;
    if (a->sin_port != b->sin_port)
        return 0;
    if (a->sin_addr.s_addr != b->sin_addr.s_addr)
        return 0;
    return 1;
}

static int parse_datagram(const char* buf, int len, ParsedMessage* out)
{
    u32 msg_len;

    if (len < 17)
        return -1;

    out->idx = read_u32_be((const unsigned char*)(buf + 0));
    out->aa = read_u16_be((const unsigned char*)(buf + 4));
    out->bbb = read_s32_be((const unsigned char*)(buf + 6));
    out->hh = (unsigned char)buf[10];
    out->mm = (unsigned char)buf[11];
    out->ss = (unsigned char)buf[12];
    msg_len = read_u32_be((const unsigned char*)(buf + 13));

    if (msg_len != (u32)(len - 17))
        return -1;

    if (out->hh > 23 || out->mm > 59 || out->ss > 59)
        return -1;

    out->text_ptr = buf + 17;
    out->text_len = msg_len;
    out->is_stop = 0;

    if (msg_len == 4 &&
        out->text_ptr[0] == 's' &&
        out->text_ptr[1] == 't' &&
        out->text_ptr[2] == 'o' &&
        out->text_ptr[3] == 'p')
    {
        out->is_stop = 1;
    }

    return 0;
}

static void msglog_unique(const char* peer, const ParsedMessage* m)
{
    FILE* f;

    f = fopen("msg.txt", "ab");
    if (!f)
        return;

    fprintf(f, "%s %u %d %02u:%02u:%02u ",
            peer,
            (unsigned int)m->aa,
            (int)m->bbb,
            (unsigned int)m->hh,
            (unsigned int)m->mm,
            (unsigned int)m->ss);

    if (m->text_len > 0)
        fwrite(m->text_ptr, 1, (size_t)m->text_len, f);

    fputc('\n', f);
    fclose(f);
}

static int ensure_client_capacity(void)
{
    ClientInfo* tmp;
    int new_cap;

    if (g_client_count < g_client_cap)
        return 0;

    new_cap = (g_client_cap == 0) ? 32 : (g_client_cap * 2);
    tmp = (ClientInfo*)realloc(g_clients, (size_t)new_cap * sizeof(ClientInfo));
    if (!tmp)
        return -1;

    g_clients = tmp;
    g_client_cap = new_cap;
    return 0;
}

static int ensure_seen_capacity(ClientInfo* c)
{
    u32* tmp;
    int new_cap;

    if (c->seen_count < c->seen_cap)
        return 0;

    new_cap = (c->seen_cap == 0) ? 32 : (c->seen_cap * 2);
    tmp = (u32*)realloc(c->seen_ids, (size_t)new_cap * sizeof(u32));
    if (!tmp)
        return -1;

    c->seen_ids = tmp;
    c->seen_cap = new_cap;
    return 0;
}

static ClientInfo* find_client(const struct sockaddr_in* addr)
{
    int i;

    for (i = 0; i < g_client_count; ++i)
    {
        if (same_client_id(&g_clients[i].addr, addr))
            return &g_clients[i];
    }

    return NULL;
}

static ClientInfo* find_or_add_client(const struct sockaddr_in* addr)
{
    ClientInfo* c;

    c = find_client(addr);
    if (c)
        return c;

    if (ensure_client_capacity() != 0)
        return NULL;

    c = &g_clients[g_client_count];
    memset(c, 0, sizeof(*c));
    c->addr = *addr;
    c->last_seen = time(NULL);
    c->seen_ids = NULL;
    c->seen_count = 0;
    c->seen_cap = 0;
    c->recent_count = 0;

    g_client_count++;
    return c;
}

static int client_has_seen(const ClientInfo* c, u32 idx)
{
    int i;

    for (i = 0; i < c->seen_count; ++i)
    {
        if (c->seen_ids[i] == idx)
            return 1;
    }

    return 0;
}

static int client_add_seen(ClientInfo* c, u32 idx)
{
    if (ensure_seen_capacity(c) != 0)
        return -1;

    c->seen_ids[c->seen_count] = idx;
    c->seen_count++;
    return 0;
}

static void client_add_recent(ClientInfo* c, u32 idx)
{
    if (c->recent_count < MAX_RECENT_ACK)
    {
        c->recent_ids[c->recent_count] = idx;
        c->recent_count++;
        return;
    }

    memmove(c->recent_ids, c->recent_ids + 1, (MAX_RECENT_ACK - 1) * sizeof(u32));
    c->recent_ids[MAX_RECENT_ACK - 1] = idx;
}

static int send_ack(SOCKET s, const struct sockaddr_in* addr, const ClientInfo* c)
{
    char buf[MAX_RECENT_ACK * 4];
    int len;
    int i;
    int rc;

    len = c->recent_count * 4;
    for (i = 0; i < c->recent_count; ++i)
        write_u32_be(buf + i * 4, c->recent_ids[i]);

    rc = sendto(s, buf, len, 0, (const struct sockaddr*)addr, sizeof(*addr));
    if (rc == SOCKET_ERROR)
    {
        print_wsa_error("sendto");
        return -1;
    }

    return 0;
}

static void remove_client_at(int idx)
{
    free(g_clients[idx].seen_ids);

    if (idx != g_client_count - 1)
        g_clients[idx] = g_clients[g_client_count - 1];

    g_client_count--;
}

static void cleanup_expired_clients(void)
{
    time_t now;
    int i;

    now = time(NULL);
    i = 0;

    while (i < g_client_count)
    {
        if ((now - g_clients[i].last_seen) >= CLIENT_TTL_SEC)
        {
            remove_client_at(i);
            continue;
        }
        i++;
    }
}

static int handle_one_datagram(SOCKET s, const char* buf, int len, const struct sockaddr_in* from)
{
    ParsedMessage m;
    ClientInfo* c;
    char peer[128];
    int is_duplicate;

    if (parse_datagram(buf, len, &m) != 0)
        return 0;

    c = find_or_add_client(from);
    if (!c)
    {
        printf("out of memory while adding client\n");
        return -1;
    }

    c->last_seen = time(NULL);
    is_duplicate = client_has_seen(c, m.idx);

    if (!is_duplicate)
    {
        if (client_add_seen(c, m.idx) != 0)
        {
            printf("out of memory while storing message id\n");
            return -1;
        }

        client_add_recent(c, m.idx);
        peer_to_string(from, peer, sizeof(peer));
        msglog_unique(peer, &m);
    }

    if (send_ack(s, from, c) != 0)
        return -1;

    if (m.is_stop)
        g_running = 0;

    return 0;
}

static int handle_readable_socket(SOCKET s)
{
    for (;;)
    {
        char buf[65535];
        struct sockaddr_in from;
        int from_len;
        int rc;

        from_len = sizeof(from);
        rc = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, &from_len);
        if (rc == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
                break;

            print_wsa_error("recvfrom");
            return -1;
        }

        if (handle_one_datagram(s, buf, rc, &from) != 0)
            return -1;

        if (!g_running)
            break;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int first_port;
    int last_port;
    int port_count;
    SOCKET* socks;
    WSAEVENT* events;
    int i;
    int rc;

    socks = NULL;
    events = NULL;
    rc = 1;

    if (argc != 3)
    {
        printf("Usage: %s first_port last_port\n", argv[0]);
        return 1;
    }

    first_port = atoi(argv[1]);
    last_port = atoi(argv[2]);

    if (first_port <= 0 || last_port <= 0 || first_port > 65535 || last_port > 65535 || first_port > last_port)
    {
        printf("invalid port range\n");
        return 1;
    }

    port_count = last_port - first_port + 1;
    if (port_count > WSA_MAXIMUM_WAIT_EVENTS)
    {
        printf("too many ports for WSAEvents, max supported in one process loop is %d\n", WSA_MAXIMUM_WAIT_EVENTS);
        return 1;
    }

    if (init_winsock() != 0)
        return 1;

    socks = (SOCKET*)malloc((size_t)port_count * sizeof(SOCKET));
    events = (WSAEVENT*)malloc((size_t)port_count * sizeof(WSAEVENT));
    if (!socks || !events)
    {
        printf("out of memory\n");
        goto cleanup;
    }

    for (i = 0; i < port_count; ++i)
    {
        struct sockaddr_in addr;

        socks[i] = INVALID_SOCKET;
        events[i] = WSA_INVALID_EVENT;
    }

    for (i = 0; i < port_count; ++i)
    {
        struct sockaddr_in addr;

        socks[i] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socks[i] == INVALID_SOCKET)
        {
            print_wsa_error("socket");
            goto cleanup;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons((u16)(first_port + i));
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(socks[i], (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            print_wsa_error("bind");
            goto cleanup;
        }

        events[i] = WSACreateEvent();
        if (events[i] == WSA_INVALID_EVENT)
        {
            print_wsa_error("WSACreateEvent");
            goto cleanup;
        }

        if (WSAEventSelect(socks[i], events[i], FD_READ) == SOCKET_ERROR)
        {
            print_wsa_error("WSAEventSelect");
            goto cleanup;
        }

        printf("Listening UDP port: %d\n", first_port + i);
    }

    while (g_running)
    {
        DWORD wait_res;
        int idx;
        WSANETWORKEVENTS ne;

        cleanup_expired_clients();

        wait_res = WSAWaitForMultipleEvents(
            (DWORD)port_count,
            events,
            FALSE,
            1000,
            FALSE
        );

        if (wait_res == WSA_WAIT_TIMEOUT)
            continue;

        if (wait_res == WSA_WAIT_FAILED)
        {
            print_wsa_error("WSAWaitForMultipleEvents");
            goto cleanup;
        }

        idx = (int)(wait_res - WSA_WAIT_EVENT_0);
        if (idx < 0 || idx >= port_count)
            continue;

        if (WSAEnumNetworkEvents(socks[idx], events[idx], &ne) == SOCKET_ERROR)
        {
            print_wsa_error("WSAEnumNetworkEvents");
            goto cleanup;
        }

        if (ne.lNetworkEvents & FD_READ)
        {
            if (handle_readable_socket(socks[idx]) != 0)
                goto cleanup;
        }
    }

    rc = 0;

cleanup:
    if (events)
    {
        for (i = 0; i < port_count; ++i)
        {
            if (events[i] != WSA_INVALID_EVENT)
                WSACloseEvent(events[i]);
        }
        free(events);
    }

    if (socks)
    {
        for (i = 0; i < port_count; ++i)
        {
            if (socks[i] != INVALID_SOCKET)
                closesocket(socks[i]);
        }
        free(socks);
    }

    if (g_clients)
    {
        for (i = 0; i < g_client_count; ++i)
            free(g_clients[i].seen_ids);
        free(g_clients);
    }

    deinit_winsock();
    return rc;
}