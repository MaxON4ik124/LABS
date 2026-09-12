
#pragma once

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <Aclapi.h>
#include <sddl.h>

#ifdef GetFreeSpace
#undef GetFreeSpace
#endif

#include <iostream>
#include <iomanip>
#include <cwchar>
#include <string>

using namespace std;

struct DiskData
{
    WCHAR Name[4];
    double FreeSpaceGb;
    UINT DriveType;
    WCHAR FileSystem[MAX_PATH];
    DiskData *Next;
};

struct FileInfo
{
    wstring Path;
    wstring OwnerSid;
    wstring OwnerName;
    DWORD AccessMask;
    wstring Access;
};

struct ServerInfo
{
    WCHAR OsType[100];
    SYSTEMTIME SysTime;
    SYSTEMTIME StartTime;
    ULONGLONG TimeSinceLaunch;
    MEMORYSTATUSEX TotalMemory;
    DiskData *DiskList;
    FileInfo FileData;
};

wstring GetOS();
SYSTEMTIME GetTime();
SYSTEMTIME GetStartTime();
ULONGLONG GetUptime();
MEMORYSTATUSEX GetRAMInfo();
DiskData *GetDrives();
void GetFreeSpace(DiskData *disks);
wstring GetAccess(const WCHAR *path);
FileInfo GetOwner(const WCHAR *path);

void get_info(ServerInfo *server);
FileInfo getFileInfo(const WCHAR *path);
void free_disks(DiskData *disk);
