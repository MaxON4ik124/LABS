#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else // LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>
#endif
#include <stdio.h>
#include <string.h>
int main()
{
int s;
struct sockaddr_in addr;
int i;
#ifdef _WIN32
int flags = 0;
#else
int flags = MSG_NOSIGNAL;
#endif
// Инициалиазация сетевой библиотеки
init();
// Создание UDP-сокета
s = socket(AF_INET, SOCK_DGRAM, 0);
if (s < 0)
return sock_err("socket", s);
// Заполнение структуры с адресом прослушивания узла
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_port = htons(8000); // Будет прослушиваться порт 8000
addr.sin_addr.s_addr = htonl(INADDR_ANY);
// Связь адреса и сокета, чтобы он мог принимать входящие дейтаграммы
if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0)
return sock_err("bind", s);
do
{
char buffer[1024] = { 0 };
int len = 0;
int addrlen = sizeof(addr);
// Принятие очередной дейтаграммы
int rcv = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*) &addr,
&addrlen);
if (rcv > 0)
{
unsigned int ip = ntohl(addr.sin_addr.s_addr);
printf("Datagram received from address: %u.%u.%u.%u ",
(ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, (ip)& 0xFF);
for (i = 0; i < rcv; i++)
{
if (buffer[i] == '\n')
break;
len++;
}
printf(" string len is: %d\n", len);
}
sprintf(buffer, "Length of your string: %d chars.", len);
// Отправка результата-дейтаграммы клиенту
sendto(s, buffer, strlen(buffer), flags, (struct sockaddr*) &addr, addrlen);
} while (1);
// Закрытие сокета
s_close(s);
deinit();
return 0;
}