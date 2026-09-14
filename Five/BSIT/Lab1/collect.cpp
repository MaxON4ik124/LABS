#include <stdexcept>
#include "collect.h"

using namespace std;

namespace
{
DWORD ReadSecurity(const WCHAR *path, SECURITY_INFORMATION information, PSID *owner, PACL *dacl,
                   PSECURITY_DESCRIPTOR *descriptor)
{
    struct Root
    {
        const WCHAR *shortName;
        const WCHAR *longName;
        HKEY key;
    };
    const Root roots[] = {{L"HKLM", L"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE},
                          {L"HKCU", L"HKEY_CURRENT_USER", HKEY_CURRENT_USER},
                          {L"HKCR", L"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT},
                          {L"HKU", L"HKEY_USERS", HKEY_USERS},
                          {L"HKCC", L"HKEY_CURRENT_CONFIG", HKEY_CURRENT_CONFIG}};
    for (const auto &root : roots)
    {
        for (const WCHAR *prefix : {root.shortName, root.longName})
        {
            size_t length = wcslen(prefix);
            if (_wcsnicmp(path, prefix, length) != 0 || (path[length] != L'\\' && path[length] != L'\0'))
            {
                continue;
            }
            HKEY key = nullptr;
            const WCHAR *subkey = path + length + (path[length] == L'\\' ? 1 : 0);
            DWORD error = RegOpenKeyExW(root.key, subkey, 0, READ_CONTROL, &key);
            if (error != ERROR_SUCCESS)
            {
                return error;
            }
            error =
                GetSecurityInfo(key, SE_REGISTRY_KEY, information, owner, nullptr, dacl, nullptr, descriptor);
            RegCloseKey(key);
            return error;
        }
    }
    return GetNamedSecurityInfoW(path, SE_FILE_OBJECT, information, owner, nullptr, dacl, nullptr,
                                 descriptor);
}
} // namespace

ULONGLONG GetUptime()
{
    return GetTickCount64();
}

wstring GetOS()
{
    HKEY key = nullptr;
    DWORD error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", 0,
                                KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key);
    if (error != ERROR_SUCCESS)
    {
        throw runtime_error("Registry error " + to_string(error));
    }
    auto readString = [key](const WCHAR *name)
    {
        WCHAR buffer[256]{};
        DWORD size = sizeof(buffer);
        if (RegGetValueW(key, nullptr, name, RRF_RT_REG_SZ, nullptr, buffer, &size) != ERROR_SUCCESS)
        {
            return wstring();
        }
        return wstring(buffer);
    };
    wstring product = readString(L"ProductName");
    wstring version = readString(L"DisplayVersion");
    if (version.empty())
    {
        version = readString(L"ReleaseId");
    }
    wstring build = readString(L"CurrentBuildNumber");
    DWORD revision = 0, size = sizeof(revision);
    bool hasRevision =
        RegGetValueW(key, nullptr, L"UBR", RRF_RT_REG_DWORD, nullptr, &revision, &size) == ERROR_SUCCESS;
    RegCloseKey(key);
    
    if (product.rfind(L"Windows 10", 0) == 0 && !build.empty() &&
        wcstoul(build.c_str(), nullptr, 10) >= 22000)
    {
        product.replace(0, 10, L"Windows 11");
    }
    if (product.empty())
    {
        throw runtime_error("Windows ProductName is unavailable");
    }
    return product + L" version=" + version + L" build=" + build +
           (hasRevision ? L"." + to_wstring(revision) : L"");
}

SYSTEMTIME GetTime()
{
    SYSTEMTIME time{};
    GetLocalTime(&time);
    return time;
}

SYSTEMTIME GetStartTime()
{
    FILETIME now{};
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER ticks{};
    ticks.LowPart = now.dwLowDateTime;
    ticks.HighPart = now.dwHighDateTime;
    ticks.QuadPart -= GetTickCount64() * 10000ULL;
    FILETIME start{ticks.LowPart, ticks.HighPart};
    SYSTEMTIME utc{}, local{};
    if (FileTimeToSystemTime(&start, &utc))
    {
        SystemTimeToTzSpecificLocalTime(nullptr, &utc, &local);
    }
    return local;
}

MEMORYSTATUSEX GetRAMInfo()
{
    MEMORYSTATUSEX memory{};
    memory.dwLength = sizeof(memory);
    if (!GlobalMemoryStatusEx(&memory))
    {
        throw runtime_error("GlobalMemoryStatusEx failed: " + to_string(GetLastError()));
    }
    return memory;
}

DiskData *GetDrives()
{
    DiskData *disks = nullptr;
    DiskData **current = &disks;

    DWORD drives = GetLogicalDrives();

    for (int i = 0; i < 26; ++i)
    {
        if (((drives >> i) & 1) == 0)
        {
            continue;
        }

        WCHAR path[4] = {static_cast<WCHAR>(L'A' + i), L':', L'\\', L'\0'};

        UINT driveType = GetDriveTypeW(path);

        DiskData *disk = new DiskData{};

        wcsncpy(disk->Name, path, 4);
        disk->DriveType = driveType;
        disk->Next = nullptr;

        DWORD serialNumber = 0;
        DWORD maximumComponentLength = 0;
        DWORD fileSystemFlags = 0;

        GetVolumeInformationW(path, nullptr, 0, &serialNumber, &maximumComponentLength, &fileSystemFlags,
                              disk->FileSystem, MAX_PATH);

        *current = disk;
        current = &disk->Next;
    }
    return disks;
}

void GetFreeSpace(DiskData *disks)
{
    for (DiskData *disk = disks; disk != nullptr; disk = disk->Next)
    {
        disk->FreeSpaceGb = -1; // Unavailable media or insufficient permissions.
        DWORD sectorsPerCluster = 0;
        DWORD bytesPerSector = 0;
        DWORD freeClusters = 0;
        DWORD totalClusters = 0;

        if (GetDiskFreeSpaceW(disk->Name, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters))
        {
            disk->FreeSpaceGb = static_cast<double>(freeClusters) * sectorsPerCluster * bytesPerSector /
                                (1024.0 * 1024.0 * 1024.0);
        }
    }
}

FileInfo GetOwner(const WCHAR *path)
{
    FileInfo result{};
    result.Path = path;
    result.AccessMask = 0;

    PSID owner = nullptr;
    PSECURITY_DESCRIPTOR securityDescriptor = nullptr;

    DWORD error = ReadSecurity(path, OWNER_SECURITY_INFORMATION, &owner, nullptr, &securityDescriptor);

    if (error != ERROR_SUCCESS)
    {
        throw runtime_error("Windows error " + to_string(error));
    }

    if (owner != nullptr)
    {
        LPWSTR ownerSidString = nullptr;

        if (ConvertSidToStringSidW(owner, &ownerSidString))
        {
            result.OwnerSid = ownerSidString;
            LocalFree(ownerSidString);
        }

        DWORD ownerNameSize = 0;
        DWORD ownerDomainSize = 0;
        SID_NAME_USE sidType;

        LookupAccountSidW(nullptr, owner, nullptr, &ownerNameSize, nullptr, &ownerDomainSize, &sidType);

        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            wstring ownerName(ownerNameSize, L'\0');
            wstring ownerDomain(ownerDomainSize, L'\0');

            if (LookupAccountSidW(nullptr, owner, ownerName.data(), &ownerNameSize, ownerDomain.data(),
                                  &ownerDomainSize, &sidType))
            {
                ownerName.resize(ownerNameSize);
                ownerDomain.resize(ownerDomainSize);
                result.OwnerName = ownerDomain.empty() ? ownerName : ownerDomain + L"\\" + ownerName;
            }
        }
    }

    LocalFree(securityDescriptor);
    return result;
}

wstring GetAccess(const WCHAR *path)
{
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    DWORD error = ReadSecurity(path, DACL_SECURITY_INFORMATION, nullptr, nullptr, &descriptor);
    if (error != ERROR_SUCCESS)
    {
        throw runtime_error("Windows error " + to_string(error));
    }
    LPWSTR text = nullptr;
    if (!ConvertSecurityDescriptorToStringSecurityDescriptorW(descriptor, SDDL_REVISION_1,
                                                              DACL_SECURITY_INFORMATION, &text, nullptr))
    {
        error = GetLastError();
        LocalFree(descriptor);
        throw runtime_error("Windows error " + to_string(error));
    }
    wstring result(text);
    LocalFree(text);
    LocalFree(descriptor);
    return result;
}

void get_info(ServerInfo *server)
{
    wstring os = GetOS();
    wcsncpy(server->OsType, os.c_str(), 100);
    server->SysTime = GetTime();
    server->StartTime = GetStartTime();
    server->TimeSinceLaunch = GetTickCount64();
    server->TotalMemory = GetRAMInfo();
    server->DiskList = GetDrives();
    GetFreeSpace(server->DiskList);
}

FileInfo getFileInfo(const WCHAR *path)
{
    FileInfo result = GetOwner(path);
    result.Access = GetAccess(path);
    return result;
}

void free_disks(DiskData *disk)
{
    while (disk != nullptr)
    {
        DiskData *next = disk->Next;
        delete disk;
        disk = next;
    }
}
