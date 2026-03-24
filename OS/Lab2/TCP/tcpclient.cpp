#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
// Директива линковщику: использовать библиотеку сокетов
#pragma comment(lib, "ws2_32.lib")
#else // LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#endif
#include <stdio.h>
#include <string.h>
#define WEBHOST "google.com"
int init()
{
    #ifdef _WIN32
        // Для Windows следует вызвать WSAStartup перед началом использования сокетов
        WSADATA wsa_data;
        return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
    #else
        return 1; // Для других ОС действий не требуется
    #endif
}
void deinit()
{
    #ifdef _WIN32
        // Для Windows следует вызвать WSACleanup в конце работы
        WSACleanup();
    #else
        // Для других ОС действий не требуется
    #endif
}
int sock_err(const char* function, int s)
{
    int err;
    #ifdef _WIN32
        err = WSAGetLastError();
    #else
        err = errno;
    #endif
    fprintf(stderr, "%s: socket error: %d\n", function, err);
    return -1;
}
void s_close(int s)
{
    #ifdef _WIN32
        closesocket(s);
    #else
        close(s);
    #endif
}
// Функция определяет IP-адрес узла по его имени.
// Адрес возвращается в сетевом порядке байтов.
unsigned int get_host_ipn(const char* name)
{
    struct addrinfo* addr = 0;
    unsigned int ip4addr = 0;
    // Функция возвращает все адреса указанного хоста
    // в виде динамического однонаправленного списка
    if (0 == getaddrinfo(name, 0, 0, &addr))
    {
        struct addrinfo* cur = addr;
        while (cur)
        {
            // Интересует только IPv4 адрес, если их несколько - то первый
            if (cur->ai_family == AF_INET)
            {
                ip4addr = ((struct sockaddr_in*) cur->ai_addr)->sin_addr.s_addr;
                break;
            }
            cur = cur->ai_next;
        }
        freeaddrinfo(addr);
    }
    return ip4addr;
}
// Отправляет http-запрос на удаленный сервер
int send_request(int s)
{
    const char* request = "GET / HTTP/1.0\r\nServer: " WEBHOST "\r\n\r\n";
    int size = strlen(request);
    int sent = 0;
    #ifdef _WIN32
    int flags = 0;
    #else
    int flags = MSG_NOSIGNAL;
    #endif
    while (sent < size)
    {
    // Отправка очередного блока данных
        int res = send(s, request + sent, size - sent, flags);
        if (res < 0) return sock_err("send", s);
        sent += res;
        printf(" %d bytes sent.\n", sent);
    }
    return 0;
}
int recv_response(int s, FILE* f)
{
    char buffer[256];
    int res;
    // Принятие очередного блока данных.
    // Если соединение будет разорвано удаленным узлом recv вернет 0
    while ((res = recv(s, buffer, sizeof(buffer), 0)) > 0)
    {
        fwrite(buffer, 1, res, f);
        printf(" %d bytes received\n", res);
    }
    if (res < 0) return sock_err("recv", s);
    return 0;
    }
int main()
{
    int s;
    struct sockaddr_in addr;
    FILE* f;
    // Инициалиазация сетевой библиотеки
    init();
    // Создание TCP-сокета
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return sock_err("socket", s);
    // Заполнение структуры с адресом удаленного узла
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = get_host_ipn(WEBHOST);
    // Установка соединения с удаленным хостом
    if (connect(s, (struct sockaddr*) &addr, sizeof(addr)) != 0)
    {
        s_close(s);
        return sock_err("connect", s);
    }
    // Отправка запроса на удаленный сервер
    send_request(s);
    // Прием результата
    f = fopen("page.html", "wb");
    recv_response(s, f);
    fclose(f);
    // Закрытие соединения
    s_close(s);
    deinit();
    return 0;
}