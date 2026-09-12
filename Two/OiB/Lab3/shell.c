#include <windows.h>
int main()
{
    WinExec("reg delete HKCU\\Test /v test /f", 0);
    return 0;
}