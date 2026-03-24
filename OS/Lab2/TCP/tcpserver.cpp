#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
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
int recv_string(int cs)
{
char buffer[512];
int curlen = 0;
int rcv;
do
{
int i;
rcv = recv(cs, buffer, sizeof(buffer), 0);
for (i = 0; i < rcv; i++)
{
if (buffer[i] == '\n')
return curlen;
curlen++;
}
if (curlen > 5000)
{
printf("input string too large\n");
return 5000;
}
} while (rcv > 0);
return curlen;
}
int send_notice(int cs, int len)
{
char buffer[1024];
int sent = 0;
int ret;
#ifdef _WIN32
int flags = 0;
#else
int flags = MSG_NOSIGNAL;
#endif
sprintf(buffer, "Length of your string: %d chars.", len);
while (sent < (int) strlen(buffer))
{
ret = send(cs, buffer + sent, strlen(buffer) - sent, flags);
if (ret <= 0)
return sock_err("send", cs);
sent += ret;
}
return 0;
}
int main()
{
int s;
struct sockaddr_in addr;
// Инициалиазация сетевой библиотеки
init();
// Создание TCP-сокета
s = socket(AF_INET, SOCK_STREAM, 0);
if (s < 0)
return sock_err("socket", s);
// Заполнение адреса прослушивания
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_port = htons(9000); // Сервер прослушивает порт 9000
addr.sin_addr.s_addr = htonl(INADDR_ANY); // Все адреса
// Связывание сокета и адреса прослушивания
if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0)
return sock_err("bind", s);
// Начало прослушивания
if (listen(s, 1) < 0)
return sock_err("listen", s);
do
{
// Принятие очередного подключившегося клиента
int addrlen = sizeof(addr);
int cs = accept(s, (struct sockaddr*) &addr, &addrlen);
unsigned int ip;
int len;
if (cs < 0)
{
sock_err("accept", s);
break;
}
// Вывод адреса клиента
ip = ntohl(addr.sin_addr.s_addr);
printf(" Client connected: %u.%u.%u.%u ",
(ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, (ip) & 0xFF);
// Прием от клиента строки
len = recv_string(cs);
// Отправка клиенту сообщения о длине полученной строки
send_notice(cs, len);
printf(" string len is: %d\n", len);
// Отключение клиента
s_close(cs);
} while (1); // Повторение этого алгоритма в беск. цикле
s_close(s);
deinit();
return 0;
}
