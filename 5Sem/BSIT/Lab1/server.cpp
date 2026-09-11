#include "server.h"

void get_info(ServerInfo* server)
{
    DWORD bufferSize = 100;
    HKEY key;

    server->OsType[0] = L'\0';

    if (RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            0,
            KEY_QUERY_VALUE,
            &key) == ERROR_SUCCESS)
    {
        RegQueryValueExW(
            key,
            L"ProductName",
            nullptr,
            nullptr,
            reinterpret_cast<LPBYTE>(server->OsType),
            &bufferSize
        );

        RegCloseKey(key);
    }

    GetLocalTime(&server->SysTime);
    GlobalMemoryStatus(&server->TotalMemory);
    server->TimeSinceLaunch = GetTickCount64();
    server->DiskList = nullptr;
    DiskData** current = &server->DiskList;

    DWORD drives = GetLogicalDrives();

    for (int i = 0; i < 26; ++i)
    {
        if (((drives >> i) & 1) == 0)
            continue;

        WCHAR path[4] = {
            static_cast<WCHAR>(L'A' + i),
            L':',
            L'\\',
            L'\0'
        };

        UINT driveType = GetDriveTypeW(path);

        if (driveType != DRIVE_FIXED)
            continue;

        DiskData* disk = new DiskData{};

        std::wcsncpy(disk->Name, path, 4);
        disk->DriveType = driveType;
        disk->Next = nullptr;

        DWORD sectorsPerCluster = 0;
        DWORD bytesPerSector = 0;
        DWORD freeClusters = 0;
        DWORD totalClusters = 0;

        if (GetDiskFreeSpaceW(
                path,
                &sectorsPerCluster,
                &bytesPerSector,
                &freeClusters,
                &totalClusters))
        {
            disk->FreeSpaceGb =
                static_cast<double>(freeClusters) *
                sectorsPerCluster *
                bytesPerSector /
                (1024.0 * 1024.0 * 1024.0);
        }

        DWORD serialNumber = 0;
        DWORD maximumComponentLength = 0;
        DWORD fileSystemFlags = 0;

        GetVolumeInformationW(
            path,
            nullptr,
            0,
            &serialNumber,
            &maximumComponentLength,
            &fileSystemFlags,
            disk->FileSystem,
            MAX_PATH
        );

        *current = disk;
        current = &disk->Next;
    }
}

FileInfo getFileInfo(const WCHAR* path)
{
    FileInfo result{};
    result.Path = path;
    result.AccessMask = 0;

    PSID owner = nullptr;
    PACL dacl = nullptr;
    PSECURITY_DESCRIPTOR securityDescriptor = nullptr;

    DWORD error = GetNamedSecurityInfoW(
        path,
        SE_FILE_OBJECT,
        OWNER_SECURITY_INFORMATION |
        DACL_SECURITY_INFORMATION,
        &owner,
        nullptr,
        &dacl,
        nullptr,
        &securityDescriptor
    );

    if (error != ERROR_SUCCESS)
    {
        std::wcerr << L"GetNamedSecurityInfoW error: "
                   << error << L'\n';

        return result;
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

        LookupAccountSidW(
            nullptr,
            owner,
            nullptr,
            &ownerNameSize,
            nullptr,
            &ownerDomainSize,
            &sidType
        );

        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            std::wstring ownerName(ownerNameSize, L'\0');
            std::wstring ownerDomain(ownerDomainSize, L'\0');

            if (LookupAccountSidW(
                    nullptr,
                    owner,
                    ownerName.data(),
                    &ownerNameSize,
                    ownerDomain.data(),
                    &ownerDomainSize,
                    &sidType))
            {
                result.OwnerName = ownerDomain + L"\\" + ownerName;
            }
        }
    }

    if (dacl != nullptr)
    {
        ACL_SIZE_INFORMATION aclInfo{};

        if (GetAclInformation(
                dacl,
                &aclInfo,
                sizeof(aclInfo),
                AclSizeInformation))
        {
            for (DWORD i = 0; i < aclInfo.AceCount; ++i)
            {
                void* aceData = nullptr;

                if (!GetAce(dacl, i, &aceData))
                    continue;

                ACE_HEADER* header =
                    static_cast<ACE_HEADER*>(aceData);

                if (header->AceType == ACCESS_ALLOWED_ACE_TYPE)
                {
                    auto* ace =
                        static_cast<ACCESS_ALLOWED_ACE*>(aceData);

                    result.AccessMask |= ace->Mask;
                }
            }
        }
    }

    LocalFree(securityDescriptor);
    return result;
}

void free_disks(DiskData* disk)
{
    while (disk != nullptr)
    {
        DiskData* next = disk->Next;
        delete disk;
        disk = next;
    }
}




int main()
{
    ServerInfo server{};

    get_info(&server);

    std::wcout << L"OS type: "
               << server.OsType << L'\n';

    std::wcout << L"System time: "
               << std::setw(2) << std::setfill(L'0')
               << server.SysTime.wDay << L"."
               << std::setw(2)
               << server.SysTime.wMonth << L"."
               << server.SysTime.wYear << L"; "
               << std::setw(2)
               << server.SysTime.wHour << L":"
               << std::setw(2)
               << server.SysTime.wMinute << L":"
               << std::setw(2)
               << server.SysTime.wSecond << L'\n';

    for (DiskData* disk = server.DiskList;
         disk != nullptr;
         disk = disk->Next)
    {
        std::wcout << L"Disk: "
                   << disk->Name
                   << L" - "
                   << disk->FreeSpaceGb
                   << L" GB, filesystem: "
                   << disk->FileSystem
                   << L'\n';
    }

    FileInfo fileInfo =
        getFileInfo(L"C:\\Data\\test.txt");

    std::wcout << L"File: "
               << fileInfo.Path << L'\n'
               << L"Owner name: "
               << fileInfo.OwnerName << L'\n'
               << L"Access mask: 0x"
               << std::hex
               << fileInfo.AccessMask
               << std::dec << L'\n';

    free_disks(server.DiskList);
    return 0;
}