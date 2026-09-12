#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <string>
#include <map>
#include <set>
#include <vector>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef int32_t  s32;

#define FILE_LOGGER "msg.txt"
#define CLIENT_TTL_SEC 30
#define MAX_RECENT_ACK 20
#define MAX_PORTS 64
#define MAX_DGRAMS_PER_SOCKET_TICK 4096

struct ParsedMessage
{
    u32 idx;
    u16 aa;
    s32 bbb;
    u8 hh;
    u8 mm;
    u8 ss;
    string message;
    int is_stop;
};

struct ClientInfo
{
    time_t last_seen;
    set<u32> seen;
    vector<u32> recent;
};

static map<string, ClientInfo> g_clients;
static SOCKET g_sockets[MAX_PORTS];
static WSAEVENT g_events[MAX_PORTS];
static int g_port_count = 0;

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

static void write_u32_be(char* p, u32 v)
{
    p[0] = (char)((v >> 24) & 0xFF);
    p[1] = (char)((v >> 16) & 0xFF);
    p[2] = (char)((v >> 8) & 0xFF);
    p[3] = (char)(v & 0xFF);
}

static string make_client_id(const struct sockaddr_in* addr)
{
    char ip[64];
    char buf[96];
    u32 host_ip;
    unsigned int b1, b2, b3, b4;
    unsigned int port;

    memset(ip, 0, sizeof(ip));
    if (inet_ntop(AF_INET, (void*)&addr->sin_addr, ip, sizeof(ip)) == NULL)
    {
        host_ip = ntohl(addr->sin_addr.s_addr);
        b1 = (host_ip >> 24) & 0xFF;
        b2 = (host_ip >> 16) & 0xFF;
        b3 = (host_ip >> 8) & 0xFF;
        b4 = host_ip & 0xFF;
        _snprintf(ip, sizeof(ip), "%u.%u.%u.%u", b1, b2, b3, b4);
        ip[sizeof(ip) - 1] = '\0';
    }

    port = (unsigned int)ntohs(addr->sin_port);
    _snprintf(buf, sizeof(buf), "%s:%u", ip, port);
    buf[sizeof(buf) - 1] = '\0';
    return string(buf);
}

static int parse_datagram(const char* buf, int len, ParsedMessage* out)
{
    u32 msg_len;
    u32 bbb_bits;

    if (len < 17)
    {
        return -1;
    }

    msg_len = read_u32_be((const unsigned char*)(buf + 13));
    if (msg_len > 65535U)
    {
        return -1;
    }

    if ((int)(17 + msg_len) != len)
    {
        return -1;
    }

    if ((u8)buf[10] > 23 || (u8)buf[11] > 59 || (u8)buf[12] > 59)
    {
        return -1;
    }

    out->idx = read_u32_be((const unsigned char*)(buf + 0));
    out->aa = read_u16_be((const unsigned char*)(buf + 4));

    bbb_bits = read_u32_be((const unsigned char*)(buf + 6));
    memcpy(&out->bbb, &bbb_bits, sizeof(out->bbb));

    out->hh = (u8)buf[10];
    out->mm = (u8)buf[11];
    out->ss = (u8)buf[12];

    out->message.assign(buf + 17, buf + 17 + msg_len);
    out->is_stop = (msg_len == 4 && memcmp(buf + 17, "stop", 4) == 0);

    return 0;
}

static int append_to_file(const string& client_id, const ParsedMessage& msg)
{
    FILE* f;

    f = fopen(FILE_LOGGER, "a");
    if (!f)
    {
        return -1;
    }

    fprintf(f,
            "%s %u %d %02u:%02u:%02u ",
            client_id.c_str(),
            (unsigned int)msg.aa,
            (int)msg.bbb,
            (unsigned int)msg.hh,
            (unsigned int)msg.mm,
            (unsigned int)msg.ss);

    if (!msg.message.empty())
    {
        fwrite(msg.message.data(), 1, msg.message.size(), f);
    }

    fputc('\n', f);
    fclose(f);
    return 0;
}

static void cleanup_old_clients(void)
{
    time_t now;
    map<string, ClientInfo>::iterator it;
    map<string, ClientInfo>::iterator next_it;

    now = time(NULL);
    it = g_clients.begin();

    while (it != g_clients.end())
    {
        next_it = it;
        ++next_it;

        if ((now - it->second.last_seen) >= CLIENT_TTL_SEC)
        {
            g_clients.erase(it);
        }

        it = next_it;
    }
}

static void push_recent(ClientInfo& c, u32 idx)
{
    if (c.recent.size() == MAX_RECENT_ACK)
    {
        c.recent.erase(c.recent.begin());
    }
    c.recent.push_back(idx);
}

static int send_ack(SOCKET s, const struct sockaddr_in* to, int tolen, const ClientInfo& c)
{
    size_t cnt;
    size_t i;
    vector<char> buf;
    int rc;

    cnt = c.recent.size();
    if (cnt > MAX_RECENT_ACK)
    {
        cnt = MAX_RECENT_ACK;
    }

    buf.resize(cnt * 4);
    for (i = 0; i < cnt; ++i)
    {
        u32 idx = c.recent[c.recent.size() - 1 - i];
        write_u32_be(&buf[i * 4], idx);
    }

    rc = sendto(
        s,
        cnt > 0 ? &buf[0] : "",
        (int)(cnt * 4),
        0,
        (const struct sockaddr*)to,
        tolen
    );

    if (rc == SOCKET_ERROR)
    {
        int err = WSAGetLastError();

        // if (err == WSAECONNRESET || err == WSAENETRESET || err == WSAEHOSTUNREACH || err == 0)
        // {
        //     return -1;
        // }

        return -1;
    }

    return 0;
}

static int handle_one_datagram(SOCKET s)
{
    char buf[65535];
    struct sockaddr_in from;
    int from_len;
    int r;
    ParsedMessage msg;
    string client_id;
    map<string, ClientInfo>::iterator it;
    int is_new;
    int ack_ok;

    memset(&from, 0, sizeof(from));
    from_len = sizeof(from);

    r = recvfrom(
        s,
        buf,
        sizeof(buf),
        0,
        (struct sockaddr*)&from,
        &from_len
    );

    if (r == SOCKET_ERROR)
    {
        int err = WSAGetLastError();

        if (err == WSAEWOULDBLOCK)
        {
            return 1;
        }

        if (err == WSAECONNRESET || err == WSAENETRESET || err == 0)
        {
            return 0;
        }

        printf("recvfrom failed: %d\n", err);
        return -1;
    }

    if (parse_datagram(buf, r, &msg) != 0)
    {
        return 0;
    }

    cleanup_old_clients();

    client_id = make_client_id(&from);
    it = g_clients.find(client_id);
    if (it == g_clients.end())
    {
        ClientInfo info;
        info.last_seen = time(NULL);
        g_clients.insert(make_pair(client_id, info));
        it = g_clients.find(client_id);
    }

    it->second.last_seen = time(NULL);

    is_new = (it->second.seen.find(msg.idx) == it->second.seen.end()) ? 1 : 0;
    if (is_new)
    {
        it->second.seen.insert(msg.idx);
        push_recent(it->second, msg.idx);

        if (append_to_file(client_id, msg) != 0)
        {
            printf("failed to write msg.txt\n");
            return -1;
        }
    }

    ack_ok = (send_ack(s, &from, from_len, it->second) == 0) ? 1 : 0;

    if (ack_ok && msg.is_stop)
    {
        return 2;
    }

    return 0;
}

static int drain_socket(SOCKET s)
{
    int budget;
    int rc;

    budget = MAX_DGRAMS_PER_SOCKET_TICK;
    while (budget-- > 0)
    {
        rc = handle_one_datagram(s);
        if (rc == 1)
        {
            break;
        }
        if (rc == 2)
        {
            return 2;
        }
        if (rc < 0)
        {
            return -1;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    WSADATA ws;
    int first_port;
    int last_port;
    int i;
    int running;
    int rc;

    if (argc != 3)
    {
        printf("Usage: udpserver <port1> <port2>\n");
        return 1;
    }

    first_port = atoi(argv[1]);
    last_port = atoi(argv[2]);

    if (first_port < 1 || last_port < 1 || first_port > 65535 || last_port > 65535)
    {
        printf("bad port range\n");
        return 1;
    }

    if (first_port > last_port)
    {
        int tmp = first_port;
        first_port = last_port;
        last_port = tmp;
    }

    g_port_count = last_port - first_port + 1;
    if (g_port_count <= 0 || g_port_count > MAX_PORTS || g_port_count > WSA_MAXIMUM_WAIT_EVENTS)
    {
        printf("too many ports\n");
        return 1;
    }


    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    for (i = 0; i < g_port_count; ++i)
    {
        struct sockaddr_in addr;

        g_sockets[i] = INVALID_SOCKET;
        g_events[i] = WSA_INVALID_EVENT;

        g_sockets[i] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (g_sockets[i] == INVALID_SOCKET)
        {
            printf("socket failed: %d\n", WSAGetLastError());
            rc = 1;
            goto cleanup;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons((u16)(first_port + i));

        if (bind(g_sockets[i], (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            printf("bind failed on port %d: %d\n", first_port + i, WSAGetLastError());
            rc = 1;
            goto cleanup;
        }

        g_events[i] = WSACreateEvent();
        if (g_events[i] == WSA_INVALID_EVENT)
        {
            printf("WSACreateEvent failed: %d\n", WSAGetLastError());
            rc = 1;
            goto cleanup;
        }

        if (WSAEventSelect(g_sockets[i], g_events[i], FD_READ) == SOCKET_ERROR)
        {
            printf("WSAEventSelect failed: %d\n", WSAGetLastError());
            rc = 1;
            goto cleanup;
        }
    }

    running = 1;
    rc = 0;

    while (running)
    {
        DWORD dw;
        int idx;
        WSANETWORKEVENTS ne;

        dw = WSAWaitForMultipleEvents(
            (DWORD)g_port_count,
            g_events,
            FALSE,
            1000,
            FALSE
        );

        if (dw == WSA_WAIT_TIMEOUT)
        {
            cleanup_old_clients();
            continue;
        }

        if (dw == WSA_WAIT_FAILED)
        {
            printf("WSAWaitForMultipleEvents failed: %d\n", WSAGetLastError());
            rc = 1;
            break;
        }

        idx = (int)(dw - WSA_WAIT_EVENT_0);
        if (idx < 0 || idx >= g_port_count)
        {
            continue;
        }

        memset(&ne, 0, sizeof(ne));
        if (WSAEnumNetworkEvents(g_sockets[idx], g_events[idx], &ne) == SOCKET_ERROR)
        {
            printf("WSAEnumNetworkEvents failed: %d\n", WSAGetLastError());
            rc = 1;
            break;
        }

        if (ne.lNetworkEvents & FD_READ)
        {
            if (ne.iErrorCode[FD_READ_BIT] != 0 &&
                ne.iErrorCode[FD_READ_BIT] != WSAECONNRESET &&
                ne.iErrorCode[FD_READ_BIT] != WSAENETRESET)
            {
                printf("FD_READ error: %d\n", ne.iErrorCode[FD_READ_BIT]);
                rc = 1;
                break;
            }

            {
                int d = drain_socket(g_sockets[idx]);
                if (d == 2)
                {
                    running = 0;
                    break;
                }
                if (d < 0)
                {
                    rc = 1;
                    break;
                }
            }
        }
    }

cleanup:
    for (i = 0; i < g_port_count; ++i)
    {
        if (g_events[i] != WSA_INVALID_EVENT)
        {
            WSACloseEvent(g_events[i]);
        }
        if (g_sockets[i] != INVALID_SOCKET)
        {
            closesocket(g_sockets[i]);
        }
    }

    WSACleanup();
    return rc;
}