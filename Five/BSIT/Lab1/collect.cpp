#include <stdexcept>
#include <sstream>
#include <vector>
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

wstring SidToString(PSID sid)
{
    LPWSTR text = nullptr;
    if (!ConvertSidToStringSidW(sid, &text))
    {
        return L"<invalid SID>";
    }
    wstring result(text);
    LocalFree(text);
    return result;
}

wstring SidToName(PSID sid)
{
    DWORD nameSize = 0;
    DWORD domainSize = 0;
    SID_NAME_USE type{};
    LookupAccountSidW(nullptr, sid, nullptr, &nameSize, nullptr, &domainSize, &type);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        return L"<unresolved>";
    }

    wstring name(nameSize, L'\0');
    wstring domain(domainSize, L'\0');
    if (!LookupAccountSidW(nullptr, sid, name.data(), &nameSize, domain.data(), &domainSize, &type))
    {
        return L"<unresolved>";
    }

    name.resize(wcslen(name.c_str()));
    domain.resize(wcslen(domain.c_str()));
    return domain.empty() ? name : domain + L"\\" + name;
}

void AppendText(wstringstream &out, bool &first, const WCHAR *text)
{
    if (!first)
    {
        out << L", ";
    }
    out << text;
    first = false;
}

wstring AceTypeName(BYTE type)
{
    switch (type)
    {
    case ACCESS_ALLOWED_ACE_TYPE:
        return L"ALLOW";
    case ACCESS_DENIED_ACE_TYPE:
        return L"DENY";
    case SYSTEM_AUDIT_ACE_TYPE:
        return L"AUDIT";
    case SYSTEM_ALARM_ACE_TYPE:
        return L"ALARM";
    case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
        return L"ALLOW_OBJECT";
    case ACCESS_DENIED_OBJECT_ACE_TYPE:
        return L"DENY_OBJECT";
    case SYSTEM_AUDIT_OBJECT_ACE_TYPE:
        return L"AUDIT_OBJECT";
    case ACCESS_ALLOWED_CALLBACK_ACE_TYPE:
        return L"ALLOW_CALLBACK";
    case ACCESS_DENIED_CALLBACK_ACE_TYPE:
        return L"DENY_CALLBACK";
    case SYSTEM_AUDIT_CALLBACK_ACE_TYPE:
        return L"AUDIT_CALLBACK";
    default:
        return L"UNKNOWN(" + to_wstring(type) + L")";
    }
}

wstring AceFlagsName(BYTE flags)
{
    wstringstream out;
    bool first = true;
    if (flags & OBJECT_INHERIT_ACE) AppendText(out, first, L"OBJECT_INHERIT");
    if (flags & CONTAINER_INHERIT_ACE) AppendText(out, first, L"CONTAINER_INHERIT");
    if (flags & NO_PROPAGATE_INHERIT_ACE) AppendText(out, first, L"NO_PROPAGATE_INHERIT");
    if (flags & INHERIT_ONLY_ACE) AppendText(out, first, L"INHERIT_ONLY");
    if (flags & INHERITED_ACE) AppendText(out, first, L"INHERITED");
    if (flags & SUCCESSFUL_ACCESS_ACE_FLAG) AppendText(out, first, L"SUCCESSFUL_ACCESS");
    if (flags & FAILED_ACCESS_ACE_FLAG) AppendText(out, first, L"FAILED_ACCESS");
    return first ? L"NONE" : out.str();
}

wstring FileMaskNames(DWORD mask)
{
    struct Bit
    {
        DWORD value;
        const WCHAR *name;
    };
    const Bit bits[] = {
        {FILE_READ_DATA, L"FILE_READ_DATA/FILE_LIST_DIRECTORY"},
        {FILE_WRITE_DATA, L"FILE_WRITE_DATA/FILE_ADD_FILE"},
        {FILE_APPEND_DATA, L"FILE_APPEND_DATA/FILE_ADD_SUBDIRECTORY"},
        {FILE_READ_EA, L"FILE_READ_EA"},
        {FILE_WRITE_EA, L"FILE_WRITE_EA"},
        {FILE_EXECUTE, L"FILE_EXECUTE/FILE_TRAVERSE"},
        {FILE_DELETE_CHILD, L"FILE_DELETE_CHILD"},
        {FILE_READ_ATTRIBUTES, L"FILE_READ_ATTRIBUTES"},
        {FILE_WRITE_ATTRIBUTES, L"FILE_WRITE_ATTRIBUTES"},
        {DELETE, L"DELETE"},
        {READ_CONTROL, L"READ_CONTROL"},
        {WRITE_DAC, L"WRITE_DAC"},
        {WRITE_OWNER, L"WRITE_OWNER"},
        {SYNCHRONIZE, L"SYNCHRONIZE"},
        {ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY"},
        {GENERIC_READ, L"GENERIC_READ"},
        {GENERIC_WRITE, L"GENERIC_WRITE"},
        {GENERIC_EXECUTE, L"GENERIC_EXECUTE"},
        {GENERIC_ALL, L"GENERIC_ALL"},
    };

    wstringstream out;
    bool first = true;
    for (const auto &bit : bits)
    {
        if ((mask & bit.value) == bit.value)
        {
            AppendText(out, first, bit.name);
        }
    }
    return first ? L"NONE" : out.str();
}

wstring BitNumbers(DWORD mask)
{
    wstringstream out;
    bool first = true;
    for (DWORD bit = 0; bit < 32; ++bit)
    {
        if ((mask & (1u << bit)) != 0)
        {
            if (!first)
            {
                out << L",";
            }
            out << bit;
            first = false;
        }
    }
    return first ? L"none" : out.str();
}

PSID AceSid(PVOID ace)
{
    auto header = static_cast<PACE_HEADER>(ace);
    switch (header->AceType)
    {
    case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
        return reinterpret_cast<PSID>(&static_cast<ACCESS_ALLOWED_OBJECT_ACE *>(ace)->SidStart);
    case ACCESS_DENIED_OBJECT_ACE_TYPE:
        return reinterpret_cast<PSID>(&static_cast<ACCESS_DENIED_OBJECT_ACE *>(ace)->SidStart);
    case SYSTEM_AUDIT_OBJECT_ACE_TYPE:
        return reinterpret_cast<PSID>(&static_cast<SYSTEM_AUDIT_OBJECT_ACE *>(ace)->SidStart);
    default:
        return reinterpret_cast<PSID>(&static_cast<ACCESS_ALLOWED_ACE *>(ace)->SidStart);
    }
}
}

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

SystemStartInfo GetStartTime()
{
    SystemStartInfo result{};
    result.UptimeMs = GetTickCount64();

    FILETIME now{};
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER ticks{};
    ticks.LowPart = now.dwLowDateTime;
    ticks.HighPart = now.dwHighDateTime;
    ticks.QuadPart -= result.UptimeMs * 10000ULL;
    FILETIME start{ticks.LowPart, ticks.HighPart};
    SYSTEMTIME utc{}, local{};
    if (FileTimeToSystemTime(&start, &utc))
    {
        if (SystemTimeToTzSpecificLocalTime(nullptr, &utc, &local))
        {
            result.StartTime = local;
        }
    }
    return result;
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
        disk->FreeSpaceGb = -1;
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
    PACL dacl = nullptr;
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    DWORD error = ReadSecurity(path, DACL_SECURITY_INFORMATION, nullptr, &dacl, &descriptor);
    if (error != ERROR_SUCCESS)
    {
        throw runtime_error("Windows error " + to_string(error));
    }

    if (dacl == nullptr)
    {
        LocalFree(descriptor);
        return L"DACL: not present";
    }

    ACL_SIZE_INFORMATION aclInfo{};
    if (!GetAclInformation(dacl, &aclInfo, sizeof(aclInfo), AclSizeInformation))
    {
        error = GetLastError();
        LocalFree(descriptor);
        throw runtime_error("Windows error " + to_string(error));
    }

    wstringstream result;
    result << L"DACL ACE count=" << aclInfo.AceCount;
    for (DWORD index = 0; index < aclInfo.AceCount; ++index)
    {
        PVOID rawAce = nullptr;
        if (!GetAce(dacl, index, &rawAce))
        {
            error = GetLastError();
            LocalFree(descriptor);
            throw runtime_error("Windows error " + to_string(error));
        }

        auto header = static_cast<PACE_HEADER>(rawAce);
        DWORD mask = reinterpret_cast<ACCESS_ALLOWED_ACE *>(rawAce)->Mask;
        PSID sid = AceSid(rawAce);

        result << L" ACE[" << index << L"] {sid=" << SidToString(sid)
               << L"; name=" << SidToName(sid)
               << L"; type=" << AceTypeName(header->AceType)
               << L"; scope=" << AceFlagsName(header->AceFlags)
               << L"; mask=" << mask << L" (0x" << hex << uppercase << mask << dec << L")"
               << L"; bits=" << BitNumbers(mask)
               << L"; rights=" << FileMaskNames(mask) << L"}";
    }

    LocalFree(descriptor);
    return result.str();
}

void get_info(ServerInfo *server)
{
    wstring os = GetOS();
    wcsncpy(server->OsType, os.c_str(), 100);
    server->SysTime = GetTime();
    SystemStartInfo systemStart = GetStartTime();
    server->StartTime = systemStart.StartTime;
    server->TimeSinceLaunch = systemStart.UptimeMs;
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
