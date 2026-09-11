
#pragma once

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <Aclapi.h>
#include <sddl.h>

#include <iostream>
#include <iomanip>
#include <cwchar>
#include <string>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")


struct DiskData
{
    WCHAR Name[4];
    double FreeSpaceGb;
    UINT DriveType;
    WCHAR FileSystem[MAX_PATH];
    DiskData* Next;
};

struct ServerInfo
{
    WCHAR OsType[100];
    SYSTEMTIME SysTime;
    ULONGLONG TimeSinceLaunch;
    MEMORYSTATUS TotalMemory;
    DiskData* DiskList;
};

struct FileInfo
{
    std::wstring Path;
    std::wstring OwnerSid;
    std::wstring OwnerName;
    DWORD AccessMask;
};

void get_info(ServerInfo* server);
FileInfo getFileInfo(const WCHAR* path);
void free_disks(DiskData* disk);
