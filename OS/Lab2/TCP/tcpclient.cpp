// #ifdef _WIN32
// #define WIN32_LEAN_AND_MEAN
// #include <windows.h>
// #include <winsock2.h>
// #include <ws2tcpip.h>
// // Директива линковщику: использовать библиотеку сокетов
// #pragma comment(lib, "ws2_32.lib")
// #else // LINUX
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <errno.h>
// #endif
// #include <stdio.h>
// #include <string.h>
// #define WEBHOST "google.com"
// int init()
// {
//     #ifdef _WIN32
//         // Для Windows следует вызвать WSAStartup перед началом использования сокетов
//         WSADATA wsa_data;
//         return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
//     #else
//         return 1; // Для других ОС действий не требуется
//     #endif
// }
// void deinit()
// {
//     #ifdef _WIN32
//         // Для Windows следует вызвать WSACleanup в конце работы
//         WSACleanup();
//     #else
//         // Для других ОС действий не требуется
//     #endif
// }
// int sock_err(const char* function, int s)
// {
//     int err;
//     #ifdef _WIN32
//         err = WSAGetLastError();
//     #else
//         err = errno;
//     #endif
//     fprintf(stderr, "%s: socket error: %d\n", function, err);
//     return -1;
// }
// void s_close(int s)
// {
//     #ifdef _WIN32
//         closesocket(s);
//     #else
//         close(s);
//     #endif
// }
// // Функция определяет IP-адрес узла по его имени.
// // Адрес возвращается в сетевом порядке байтов.
// unsigned int get_host_ipn(const char* name)
// {
//     struct addrinfo* addr = 0;
//     unsigned int ip4addr = 0;
//     // Функция возвращает все адреса указанного хоста
//     // в виде динамического однонаправленного списка
//     if (0 == getaddrinfo(name, 0, 0, &addr))
//     {
//         struct addrinfo* cur = addr;
//         while (cur)
//         {
//             // Интересует только IPv4 адрес, если их несколько - то первый
//             if (cur->ai_family == AF_INET)
//             {
//                 ip4addr = ((struct sockaddr_in*) cur->ai_addr)->sin_addr.s_addr;
//                 break;
//             }
//             cur = cur->ai_next;
//         }
//         freeaddrinfo(addr);
//     }
//     return ip4addr;
// }
// // Отправляет http-запрос на удаленный сервер
// int send_request(int s)
// {
//     const char* request = "GET / HTTP/1.0\r\nServer: " WEBHOST "\r\n\r\n";
//     int size = strlen(request);
//     int sent = 0;
//     #ifdef _WIN32
//     int flags = 0;
//     #else
//     int flags = MSG_NOSIGNAL;
//     #endif
//     while (sent < size)
//     {
//     // Отправка очередного блока данных
//         int res = send(s, request + sent, size - sent, flags);
//         if (res < 0) return sock_err("send", s);
//         sent += res;
//         printf(" %d bytes sent.\n", sent);
//     }
//     return 0;
// }
// int recv_response(int s, FILE* f)
// {
//     char buffer[256];
//     int res;
//     // Принятие очередного блока данных.
//     // Если соединение будет разорвано удаленным узлом recv вернет 0
//     while ((res = recv(s, buffer, sizeof(buffer), 0)) > 0)
//     {
//         fwrite(buffer, 1, res, f);
//         printf(" %d bytes received\n", res);
//     }
//     if (res < 0) return sock_err("recv", s);
//     return 0;
//     }
// int main()
// {
//     int s;
//     struct sockaddr_in addr;
//     FILE* f;
//     // Инициалиазация сетевой библиотеки
//     init();
//     // Создание TCP-сокета
//     s = socket(AF_INET, SOCK_STREAM, 0);
//     if (s < 0) return sock_err("socket", s);
//     // Заполнение структуры с адресом удаленного узла
//     memset(&addr, 0, sizeof(addr));
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(80);
//     addr.sin_addr.s_addr = get_host_ipn(WEBHOST);
//     // Установка соединения с удаленным хостом
//     if (connect(s, (struct sockaddr*) &addr, sizeof(addr)) != 0)
//     {
//         s_close(s);
//         return sock_err("connect", s);
//     }
//     // Отправка запроса на удаленный сервер
//     send_request(s);
//     // Прием результата
//     f = fopen("page.html", "wb");
//     recv_response(s, f);
//     fclose(f);
//     // Закрытие соединения
//     s_close(s);
//     deinit();
//     return 0;
// }

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#pragma comment(lib, "Ws2_32.lib")

static void print_wsa_error(const char* where)
{
    int err = WSAGetLastError();
    fprintf(stderr, "%s failed. WSA error = %d\n", where, err);
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

static int send_all(SOCKET s, const void* data, size_t len)
{
    const char* p = (const char*)data;

    while (len > 0)
    {
        int chunk = (len > INT_MAX) ? INT_MAX : (int)len;
        int sent = send(s, p, chunk, 0);
        if (sent == SOCKET_ERROR)
        {
            print_wsa_error("send");
            return -1;
        }
        if (sent == 0)
        {
            fprintf(stderr, "send returned 0, connection closed unexpectedly\n");
            return -1;
        }

        p += sent;
        len -= sent;
    }

    return 0;
}

static int recv_all(SOCKET s, void* data, size_t len)
{
    char* p = (char*)data;
    size_t received_total = 0;

    while (received_total < len)
    {
        int chunk = (len - received_total > INT_MAX) ? INT_MAX : (int)(len - received_total);
        int received = recv(s, p + received_total, chunk, 0);

        if (received == SOCKET_ERROR)
        {
            print_wsa_error("recv");
            return -1;
        }
        if (received == 0)
        {
            fprintf(stderr, "server closed connection unexpectedly\n");
            return -1;
        }

        received_total += (size_t)received;
    }

    return 0;
}

/* Чтение строки любой длины */
static int read_line_alloc(FILE* f, char** out_line)
{
    size_t cap = 256;
    size_t len = 0;
    int ch;
    char* buf = (char*)malloc(cap);

    if (!buf)
        return -1;

    while ((ch = fgetc(f)) != EOF)
    {
        if (ch == '\n')
            break;

        if (ch == '\r')
        {
            int next = fgetc(f);
            if (next != '\n' && next != EOF)
                ungetc(next, f);
            break;
        }

        if (len + 1 >= cap)
        {
            size_t new_cap = cap * 2;
            char* tmp = (char*)realloc(buf, new_cap);
            if (!tmp)
            {
                free(buf);
                return -1;
            }
            buf = tmp;
            cap = new_cap;
        }

        buf[len++] = (char)ch;
    }

    if (ch == EOF && len == 0)
    {
        free(buf);
        *out_line = NULL;
        return 0; /* EOF */
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

/*
 * Формат строки:
 * AA BBB hh:mm:ss Message
 */
static int parse_line(
    const char* line,
    uint16_t* out_aa,
    int32_t* out_bbb,
    unsigned char out_time[3],
    char** out_msg,
    size_t* out_msg_len)
{
    const char* p = line;
    char* endptr;
    unsigned long aa;
    long bbb;

    /* UTF-8 BOM в первой строке, если вдруг файл сохранен с BOM */
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
    if (*endptr != ' ')
        return -1;
    p = endptr + 1;

    /* hh:mm:ss */
    if (strlen(p) < 8)
        return -1;

    int hh = parse_two_digits(p);
    if (hh < 0 || p[2] != ':')
        return -1;

    int mm = parse_two_digits(p + 3);
    if (mm < 0 || p[5] != ':')
        return -1;

    int ss = parse_two_digits(p + 6);
    if (ss < 0)
        return -1;

    if (hh > 23 || mm > 59 || ss > 59)
        return -1;

    p += 8;
    if (*p != ' ')
        return -1;
    p++;

    *out_aa = (uint16_t)aa;
    *out_bbb = (int32_t)bbb;
    out_time[0] = (unsigned char)hh;
    out_time[1] = (unsigned char)mm;
    out_time[2] = (unsigned char)ss;

    *out_msg_len = strlen(p);
    *out_msg = (char*)malloc(*out_msg_len + 1);
    if (!*out_msg)
        return -1;

    memcpy(*out_msg, p, *out_msg_len + 1);
    return 0;
}

static int build_packet(
    uint32_t idx,
    uint16_t aa,
    int32_t bbb,
    const unsigned char tm[3],
    const char* msg,
    size_t msg_len,
    unsigned char** out_buf,
    size_t* out_len)
{
    size_t packet_len = 4 + 2 + 4 + 3 + 4 + msg_len;
    unsigned char* buf = (unsigned char*)malloc(packet_len);
    uint32_t idx_net;
    uint16_t aa_net;
    uint32_t bbb_bits;
    uint32_t bbb_net;
    uint32_t msg_len_net;

    if (!buf)
        return -1;

    idx_net = htonl(idx);
    aa_net = htons(aa);

    /* сохранить битовый образ signed int32 */
    bbb_bits = (uint32_t)bbb;
    bbb_net = htonl(bbb_bits);

    if (msg_len > 0xFFFFFFFFu)
    {
        free(buf);
        return -1;
    }
    msg_len_net = htonl((uint32_t)msg_len);

    memcpy(buf + 0,  &idx_net,     4);
    memcpy(buf + 4,  &aa_net,      2);
    memcpy(buf + 6,  &bbb_net,     4);
    memcpy(buf + 10, tm,           3);
    memcpy(buf + 13, &msg_len_net, 4);
    memcpy(buf + 17, msg,          msg_len);

    *out_buf = buf;
    *out_len = packet_len;
    return 0;
}

static SOCKET connect_to_server(const char* ip, uint16_t port)
{
    SOCKET s = INVALID_SOCKET;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (InetPtonA(AF_INET, ip, &addr.sin_addr) != 1)
    {
        fprintf(stderr, "Invalid IPv4 address: %s\n", ip);
        return INVALID_SOCKET;
    }

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        print_wsa_error("socket");
        return INVALID_SOCKET;
    }

    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        print_wsa_error("connect");
        closesocket(s);
        return INVALID_SOCKET;
    }

    return s;
}

int main(int argc, char* argv[])
{
    const char* server_ip;
    const char* file_name;
    uint16_t port;
    char* endptr;
    unsigned long port_ul;
    SOCKET sock = INVALID_SOCKET;
    FILE* f = NULL;
    uint32_t idx = 0;
    int rc = 1;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s xx.xx.xx.xx pppp file\n", argv[0]);
        return 1;
    }

    server_ip = argv[1];
    file_name = argv[3];

    port_ul = strtoul(argv[2], &endptr, 10);
    if (*argv[2] == '\0' || *endptr != '\0' || port_ul == 0 || port_ul > 65535)
    {
        fprintf(stderr, "Invalid port: %s\n", argv[2]);
        return 1;
    }
    port = (uint16_t)port_ul;

    if (init_winsock() != 0)
        return 1;

    sock = connect_to_server(server_ip, port);
    if (sock == INVALID_SOCKET)
        goto cleanup;

    /* Режим работы клиента: put */
    if (send_all(sock, "put", 3) != 0)
        goto cleanup;

    f = fopen(file_name, "rb");
    if (!f)
    {
        fprintf(stderr, "Cannot open file: %s\n", file_name);
        goto cleanup;
    }

    for (;;)
    {
        char* line = NULL;
        int line_status = read_line_alloc(f, &line);

        if (line_status < 0)
        {
            fprintf(stderr, "Failed to read line from file\n");
            goto cleanup;
        }
        if (line_status == 0)
        {
            /* EOF */
            break;
        }

        if (line[0] == '\0')
        {
            free(line);
            continue; /* пустые строки пропускаем */
        }

        uint16_t aa;
        int32_t bbb;
        unsigned char tm[3];
        char* msg = NULL;
        size_t msg_len = 0;
        unsigned char* packet = NULL;
        size_t packet_len = 0;
        char ack[2];

        if (parse_line(line, &aa, &bbb, tm, &msg, &msg_len) != 0)
        {
            fprintf(stderr, "Invalid line format at message #%u:\n%s\n", idx, line);
            free(line);
            goto cleanup;
        }

        if (build_packet(idx, aa, bbb, tm, msg, msg_len, &packet, &packet_len) != 0)
        {
            fprintf(stderr, "Failed to build packet for message #%u\n", idx);
            free(msg);
            free(line);
            goto cleanup;
        }

        if (send_all(sock, packet, packet_len) != 0)
        {
            free(packet);
            free(msg);
            free(line);
            goto cleanup;
        }

        /* Сервер после каждого сообщения шлет "ok" */
        if (recv_all(sock, ack, 2) != 0)
        {
            free(packet);
            free(msg);
            free(line);
            goto cleanup;
        }

        if (ack[0] != 'o' || ack[1] != 'k')
        {
            fprintf(stderr, "Unexpected server reply after message #%u: %02X %02X\n",
                    idx, (unsigned char)ack[0], (unsigned char)ack[1]);
            free(packet);
            free(msg);
            free(line);
            goto cleanup;
        }

        printf("Message #%u sent successfully\n", idx);

        free(packet);
        free(msg);
        free(line);
        idx++;
    }

    shutdown(sock, SD_SEND);
    printf("%u messages sent.\n", idx);
    rc = 0;

cleanup:
    if (f)
        fclose(f);
    if (sock != INVALID_SOCKET)
        closesocket(sock);
    deinit_winsock();
    return rc;
}