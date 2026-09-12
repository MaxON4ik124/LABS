#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else // LINUX
#include <fcntl.h>
#endif
int set_non_block_mode(int s)
{
#ifdef _WIN32
unsigned long mode = 1;
return ioctlsocket(s, FIONBIO, &mode);
#else
int fl = fcntl(s, F_GETFL, 0);
return fcntl(s, F_SETFL, fl | O_NONBLOCK);
#endif
}