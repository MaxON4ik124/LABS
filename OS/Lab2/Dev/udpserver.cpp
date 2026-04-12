#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#define MAX_PORTS 512
#define MAX_CLIENTS 4096
#define BUF_SIZE 65536
#define MAX_DGRAMS_PER_SOCKET_TICK 4096

typedef struct Client {
    uint32_t ip;
    uint16_t port;
    uint32_t* nums;
    size_t nums_cnt;
    size_t nums_cap;
    uint32_t recent[20];
    size_t recent_cnt;
    size_t recent_pos;
} Client;

typedef struct Rec {
    size_t cli_idx;
    uint32_t num;
    char* tail;
} Rec;

static Client clients[MAX_CLIENTS];
static size_t client_cnt = 0;
static Rec* recs = NULL;
static size_t rec_cnt = 0;
static size_t rec_cap = 0;

static int net_init(void) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return -1;
    }
    return 0;
}

static void net_deinit(void) {
    WSACleanup();
}

static int set_nonblock(SOCKET s) {
    u_long mode = 1;
    return (ioctlsocket(s, FIONBIO, &mode) == 0) ? 0 : -1;
}

static Client* get_client(uint32_t ip, uint16_t port) {
    size_t i;

    for (i = 0; i < client_cnt; ++i) {
        if (clients[i].ip == ip && clients[i].port == port) {
            return &clients[i];
        }
    }

    if (client_cnt >= MAX_CLIENTS) {
        return NULL;
    }

    memset(&clients[client_cnt], 0, sizeof(clients[client_cnt]));
    clients[client_cnt].ip = ip;
    clients[client_cnt].port = port;
    return &clients[client_cnt++];
}

static int has_num(const Client* c, uint32_t n) {
    size_t i;

    for (i = 0; i < c->nums_cnt; ++i) {
        if (c->nums[i] == n) {
            return 1;
        }
    }

    return 0;
}

static int push_num(Client* c, uint32_t n) {
    uint32_t* p;
    size_t cap;

    if (c->nums_cnt < c->nums_cap) {
        c->nums[c->nums_cnt++] = n;
        return 0;
    }

    cap = c->nums_cap ? c->nums_cap * 2 : 64;
    p = (uint32_t*)realloc(c->nums, cap * sizeof(uint32_t));
    if (!p) {
        return -1;
    }

    c->nums = p;
    c->nums_cap = cap;
    c->nums[c->nums_cnt++] = n;
    return 0;
}

static void push_recent(Client* c, uint32_t n) {
    c->recent[c->recent_pos] = n;
    c->recent_pos = (c->recent_pos + 1) % 20;
    if (c->recent_cnt < 20) {
        c->recent_cnt++;
    }
}

static int send_ack(SOCKET s, const struct sockaddr_in* to, int tolen, Client* c) {
    size_t cnt = c->recent_cnt;
    size_t start = (cnt < 20) ? 0 : c->recent_pos;
    uint32_t ack[20];
    size_t i;
    int rc;

    for (i = 0; i < cnt; ++i) {
        size_t idx = (start + i) % 20;
        ack[i] = htonl(c->recent[idx]);
    }

    if (cnt > 0) {
        rc = sendto(
            s,
            (const char*)ack,
            (int)(cnt * 4),
            MSG_NOSIGNAL,
            (const struct sockaddr*)to,
            tolen
        );
    } else {
        rc = sendto(
            s,
            "",
            0,
            MSG_NOSIGNAL,
            (const struct sockaddr*)to,
            tolen
        );
    }

    return (rc == SOCKET_ERROR) ? -1 : 0;
}

static int push_rec(size_t cli_idx,
                    uint32_t num,
                    const char* p1,
                    const char* p2,
                    unsigned hh,
                    unsigned mm,
                    unsigned ss,
                    const char* msg) {
    Rec* grown;
    char* tail;
    int n;

    n = snprintf(NULL, 0, "%s %s %02u:%02u:%02u %s", p1, p2, hh, mm, ss, msg);
    if (n < 0) {
        return -1;
    }

    tail = (char*)malloc((size_t)n + 1);
    if (!tail) {
        return -1;
    }

    snprintf(tail, (size_t)n + 1, "%s %s %02u:%02u:%02u %s", p1, p2, hh, mm, ss, msg);

    if (rec_cnt == rec_cap) {
        rec_cap = rec_cap ? rec_cap * 2 : 256;
        grown = (Rec*)realloc(recs, rec_cap * sizeof(Rec));
        if (!grown) {
            free(tail);
            return -1;
        }
        recs = grown;
    }

    recs[rec_cnt].cli_idx = cli_idx;
    recs[rec_cnt].num = num;
    recs[rec_cnt].tail = tail;
    rec_cnt++;
    return 0;
}

static int rec_cmp(const void* a, const void* b) {
    const Rec* ra = (const Rec*)a;
    const Rec* rb = (const Rec*)b;

    if (ra->cli_idx < rb->cli_idx) return -1;
    if (ra->cli_idx > rb->cli_idx) return 1;
    if (ra->num < rb->num) return -1;
    if (ra->num > rb->num) return 1;
    return 0;
}

int main(int argc, char** argv) {
    int first_port;
    int last_port;
    int nports;
    int i;
    SOCKET socks[MAX_PORTS];
    FILE* out;
    int running = 1;

    if (argc != 3) {
        fprintf(stderr, "usage: udpserver first_port last_port\n");
        return 1;
    }

    first_port = atoi(argv[1]);
    last_port = atoi(argv[2]);
    if (first_port < 1 || last_port < first_port || last_port > 65535) {
        fprintf(stderr, "bad port range\n");
        return 1;
    }

    nports = last_port - first_port + 1;
    if (nports > MAX_PORTS) {
        fprintf(stderr, "too many ports\n");
        return 1;
    }

    if (net_init() != 0) {
        return 1;
    }

    out = fopen("msg.txt", "w");
    if (!out) {
        fprintf(stderr, "msg.txt open failed\n");
        net_deinit();
        return 1;
    }

    for (i = 0; i < nports; ++i) {
        SOCKET s;
        struct sockaddr_in a;
        int sz = 1 << 20;

        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s == INVALID_SOCKET) {
            fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
            fclose(out);
            net_deinit();
            return 1;
        }

        setsockopt(s, SOL_SOCKET, SO_RCVBUF, (const char*)&sz, sizeof(sz));
        setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const char*)&sz, sizeof(sz));

        if (set_nonblock(s) < 0) {
            fprintf(stderr, "ioctlsocket(FIONBIO) failed: %d\n", WSAGetLastError());
            closesocket(s);
            fclose(out);
            net_deinit();
            return 1;
        }

        memset(&a, 0, sizeof(a));
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_ANY);
        a.sin_port = htons((uint16_t)(first_port + i));

        if (bind(s, (struct sockaddr*)&a, sizeof(a)) == SOCKET_ERROR) {
            fprintf(stderr, "bind failed: %d\n", WSAGetLastError());
            closesocket(s);
            fclose(out);
            net_deinit();
            return 1;
        }

        socks[i] = s;
    }

    while (running) {
        fd_set rf;
        struct timeval tv;
        int sr;

        FD_ZERO(&rf);
        for (i = 0; i < nports; ++i) {
            FD_SET(socks[i], &rf);
        }

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        sr = select(0, &rf, NULL, NULL, &tv);
        if (sr == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAEINTR) {
                continue;
            }
            fprintf(stderr, "select failed: %d\n", err);
            break;
        }

        if (sr == 0) {
            continue;
        }

        for (i = 0; i < nports && running; ++i) {
            if (!FD_ISSET(socks[i], &rf)) {
                continue;
            }

            {
                int budget = MAX_DGRAMS_PER_SOCKET_TICK;

                while (budget-- > 0) {
                    char buf[BUF_SIZE];
                    struct sockaddr_in from;
                    int fl = sizeof(from);
                    int r;
                    Client* c;
                    size_t cli_idx;
                    uint32_t num;
                    size_t j;
                    int zero = -1;
                    char p1[13], p2[13];
                    unsigned hh, mm, ss;
                    int fresh = 0;
                    int ack_ok = 0;
                    const char* msg;

                    r = recvfrom(
                        socks[i],
                        buf,
                        sizeof(buf),
                        0,
                        (struct sockaddr*)&from,
                        &fl
                    );

                    if (r == SOCKET_ERROR) {
                        int err = WSAGetLastError();
                        if (err == WSAEWOULDBLOCK) {
                            break;
                        }
                        fprintf(stderr, "recvfrom failed: %d\n", err);
                        break;
                    }

                    if (r == 3 && buf[0] == 'p' && buf[1] == 'u' && buf[2] == 't') {
                        const char ok[2] = { 'o', 'k' };
                        sendto(
                            socks[i],
                            ok,
                            2,
                            MSG_NOSIGNAL,
                            (struct sockaddr*)&from,
                            fl
                        );
                        continue;
                    }

                    if (r < 32) {
                        continue;
                    }

                    for (j = 31; j < (size_t)r; ++j) {
                        if (buf[j] == '\0') {
                            zero = (int)j;
                            break;
                        }
                    }

                    if (zero < 0) {
                        continue;
                    }

                    memcpy(&num, buf, 4);
                    num = ntohl(num);

                    c = get_client(from.sin_addr.s_addr, ntohs(from.sin_port));
                    if (!c) {
                        continue;
                    }

                    cli_idx = (size_t)(c - clients);

                    if (!has_num(c, num)) {
                        if (push_num(c, num) != 0) {
                            continue;
                        }
                        fresh = 1;
                    }

                    push_recent(c, num);

                    memcpy(p1, buf + 4, 12);
                    p1[12] = 0;

                    memcpy(p2, buf + 16, 12);
                    p2[12] = 0;

                    hh = (unsigned char)buf[28];
                    mm = (unsigned char)buf[29];
                    ss = (unsigned char)buf[30];
                    msg = buf + 31;

                    if (fresh) {
                        if (push_rec(cli_idx, num, p1, p2, hh, mm, ss, msg) != 0) {
                            continue;
                        }
                    }

                    if (send_ack(socks[i], &from, fl, c) == 0) {
                        ack_ok = 1;
                    }

                    if (ack_ok && strcmp(msg, "stop") == 0) {
                        running = 0;
                        break;
                    }
                }
            }
        }
    }

    qsort(recs, rec_cnt, sizeof(Rec), rec_cmp);

    for (i = 0; i < (int)rec_cnt; ++i) {
        uint32_t ip_h = ntohl(clients[recs[i].cli_idx].ip);
        uint16_t p = clients[recs[i].cli_idx].port;

        fprintf(
            out,
            "%u.%u.%u.%u:%u %s\n",
            (unsigned)((ip_h >> 24) & 255),
            (unsigned)((ip_h >> 16) & 255),
            (unsigned)((ip_h >> 8) & 255),
            (unsigned)(ip_h & 255),
            (unsigned)p,
            recs[i].tail
        );
    }

    fclose(out);

    for (i = 0; i < nports; ++i) {
        closesocket(socks[i]);
    }

    for (i = 0; i < (int)client_cnt; ++i) {
        free(clients[i].nums);
    }

    for (i = 0; i < (int)rec_cnt; ++i) {
        free(recs[i].tail);
    }

    free(recs);
    net_deinit();
    return 0;
}