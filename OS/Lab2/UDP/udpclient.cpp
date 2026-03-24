#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
// Директива линковщику: использовать библиотеку сокетов
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
// Функция извлекает IPv4-адрес из DNS-дейтаграммы.
// Задание л/р не требует детального изучения кода этой функции
unsigned int get_addr_from_dns_datagram(const char* datagram, int size);
void send_request(int s, struct sockaddr_in* addr)
{
// Данные дейтаграммы DNS-запроса. Детальное изучение для л/р не требуется.
char dns_datagram[] = {0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
3, 'w', 'w','w', 6, 'y','a','n','d','e','x', 2, 'r', 'u', 0,
0x00, 0x01, 0x00, 0x01};
#ifdef _WIN32
int flags = 0;
#else
int flags = MSG_NOSIGNAL;
#endif
int res = sendto(s, dns_datagram, sizeof(dns_datagram), flags, (struct sockaddr*) addr,
sizeof(struct sockaddr_in));
if (res <= 0)
sock_err("sendto", s);
}
// Функция принимает дейтаграмму от удаленной стороны.
// Возвращает 0, если в течение 100 миллисекунд не было получено ни одной дейтаграммы
unsigned int recv_response(int s)
{
char datagram[1024];
struct timeval tv = {0, 100*1000}; // 100 msec
int res;
fd_set fds;
FD_ZERO(&fds); FD_SET(s, &fds);
// Проверка - если в сокете входящие дейтаграммы
// (ожидание в течение tv)
res = select(s + 1, &fds, 0, 0, &tv);
if (res > 0)
{
// Данные есть, считывание их
struct sockaddr_in addr;
int addrlen = sizeof(addr);
int received = recvfrom(s, datagram, sizeof(datagram), 0, (struct sockaddr*) &addr,
&addrlen);
if (received <= 0)
{
// Ошибка считывания полученной дейтаграммы
sock_err("recvfrom", s);
return 0;
}
return get_addr_from_dns_datagram(datagram, sizeof(datagram));
}
else if (res == 0)
{
// Данных в сокете нет, возврат ошибки
return 0;
}
else
{
sock_err("select", s);
return 0;
}
}
int main()
{
int s;
struct sockaddr_in addr;
int i;
// Инициалиазация сетевой библиотеки
init();
// Создание UDP-сокета
s = socket(AF_INET, SOCK_DGRAM, 0);
if (s < 0)
return sock_err("socket", s);
// Заполнение структуры с адресом удаленного узла
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_port = htons(53); // Порт DNS - 53
addr.sin_addr.s_addr = inet_addr("8.8.8.8");
// Выполняется 5 попыток отправки и затем получения дейтаграммы.
// Если запрос или ответ будет потерян - данные будут запрошены повторно
for (i = 0; i < 5; i++)
{
printf(" sending request: attempt %d\n", i + 1);
// Отправка запроса на удаленный сервер
send_request(s, &addr);
// Попытка получить ответ. Если ответ получен - завершение цикла попыток
if (recv_response(s))
{
break;
}
}
// Закрытие сокета
s_close(s);
deinit();
return 0;
}
unsigned int get_addr_from_dns_datagram(const char* datagram, int size)
{
unsigned short req_cnt, ans_cnt, i;
const char* ptr;
req_cnt = ntohs(*(unsigned short*)(datagram + 4));
ans_cnt = ntohs(*(unsigned short*)(datagram + 6));
ptr = datagram + 12;
for (i = 0; i < req_cnt; i++)
{
unsigned char psz;
do
{
psz = *ptr;
ptr += psz + 1;
} while (psz > 0);
ptr += 4;
}
for (i = 0; i < ans_cnt; i++)
{
unsigned char psz;
unsigned short asz;
do
{
psz = *ptr;
if (psz & 0xC0)
{
ptr += 2;
break;
}
ptr += psz + 1;
} while (psz > 0);
ptr += 8;
asz = ntohs(*(unsigned short*)ptr);
if (asz == 4)
{
printf(" Found IP: %u.%u.%u.%u\n",
(unsigned char)ptr[1], (unsigned char)ptr[2], (unsigned char)ptr[3],
(unsigned char)ptr[4]);
}
ptr += 2 + asz;
}
return 1;
}