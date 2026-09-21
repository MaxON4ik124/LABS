#include "main.h"

char buf[200];

void encrypt(const char* source, size_t size)
{
    size_t keylen = strlen(KEY);
    size_t keyind = 0;
    size_t output_size = size < BUFSIZE ? size : BUFSIZE - 1;

    memset(buf, 0, BUFSIZE);

    for(size_t i = 0; i < output_size; i++)
    {
        while(isPrime((int)(keyind % keylen)))
            keyind++;
        buf[i] = source[i] ^ KEY[keyind % keylen];
        keyind++;
    }
    buf[output_size] = '\0';
}

bool CheckPass(char* PwdInp)
{
    encrypt(PASSWORD, strlen(PASSWORD));
    if(strcmp(buf, PwdInp) == 0)
    {
        memset(buf, 0, BUFSIZE);
        return true;
    }
    memset(buf, 0, BUFSIZE);
    return false;
}

bool isPrime(int num)
{
    if(num <= 3) return true;
    for(int i = 2;i <= num / 2;i++)
        if(num % i == 0) return false;
    return true;
}

uint32_t calcCRC(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    uint32_t crc = 0xFFFFFFFF;
    size_t read;
    unsigned char buff[4096];
    while((read = fread(buff, 1, sizeof(buff), file)) > 0)
    {
        for(size_t i = 0; i < read; i++)
        {
            crc ^= buff[i];

            for(int bit = 0; bit < 8;bit++)
            {
                if(crc & 1)
                    crc = (crc >> 1) ^ POLYNOMINAL;
                else
                    crc >>= 1;
            }
        }
    }

    fclose(file);
    return crc ^ 0xFFFFFFFF;
}

bool checkCRC()
{
    char exe[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exe, MAX_PATH);
    FILE* expectedCRC = fopen("Assets\\Sec\\checksum.txt", "r");

    uint32_t expected_crc;

    if(fscanf(expectedCRC, "%x", &expected_crc) != 1)
    {
        fclose(expectedCRC);
        return false;
    }
    fclose(expectedCRC);
    
    if(len == 0 || len >= MAX_PATH)
        return false;
    return calcCRC(exe) == expected_crc;
}

void serial()
{
    
    if(CheckIDP() || CheckRemoteDebug() || CheckTicks() || CheckNtGlobalFlag() || CheckWindows())
    {
        fprintf(stdout, "Debugger detected!\n");
        exit(1);
    }
    if(CheckVMFiles() || CheckVMKey() || CheckBiosVM())
    {
        fprintf(stdout, "VM is detected!\n");
        exit(1);
    }
    if(!checkCRC())
    {
        fprintf(stdout, "CRC does not match!\n");
        exit(1);
    }
    if(!CheckPass(attempt))
    {
        fprintf(stdout, "Wrong password!\n");
        exit(1);
    }
}

bool checkPass(char* PwdInp)
{
    if(strcmp(PwdInp, PASSWORD) != 0) return Check_pass(PwdInp);
    return Checkpass(PwdInp);
}

bool Checkpass(char* PwdInp)
{
    if(strcmp(PwdInp, PASSWORD) != 0) return check_pass(PwdInp);
    return check_pass(PwdInp);
}

bool checkpass(char* PwdInp)
{
    if(strcmp(PwdInp, PASSWORD) != 0) return Check_pass(PwdInp);
    return checkPass(PwdInp);
}

bool Check_Pass(char* PwdInp)
{
    if(strcmp(PwdInp, PASSWORD) != 0) return Checkpass(PwdInp);
    return Checkpass(PwdInp);
}

bool check_Pass(char* PwdInp)
{
    if(strcmp(PwdInp, PASSWORD) != 0) return false;
    return false;
}

bool Check_pass(char* PwdInp)
{
    if(strcmp(PwdInp, PASSWORD) != 0) return check_Pass(PwdInp);
    return check_Pass(PwdInp);
}


bool check_pass(char* PwdInp)
{
    if(strcmp(PwdInp, PASSWORD) != 0) return Check_pass(PwdInp);
    return Check_pass(PwdInp);
}

bool CheckIDP()
{
    if(IsDebuggerPresent())
        return true;
    else return false;
}

bool CheckRemoteDebug()
{
    BOOL res = FALSE;
    BOOL is_debug = CheckRemoteDebuggerPresent(GetCurrentProcess(), &res);
    if(res && is_debug) return true;
    else return false;
}

bool CheckTicks()
{
    int j = 0;
    DWORD time = GetTickCount64();
    for(int i = 0;i < 10;i++)
        j++;
    time = GetTickCount64() - time;
    if(time > 0x10) return true;
    else return false;
}

bool CheckNtGlobalFlag()
{
    uintptr_t peb = (uintptr_t)__readgsqword(0x60);
    DWORD nt_global_flag = *(DWORD*)(peb + 0xBC);
    return (nt_global_flag & 0x70) != 0;
}

bool CheckWindows()
{
    char* debuggers[] =
    {
        "\x48\x7d\x4\x3\x52\x2d\x70",
        "\x48\x78\x2\x3\x52\x2d\x70",
        "\x79\xf\x71\x6d",
        "\x79\xf\x71\x47\x60\x38\x15\x5b",
        "\x67\x22\x5e\x23\x52\x2d\x70",
        "\x7f\x27\x5c\x1e\x74\x28\x1d\x5b",
    };
    const size_t cnt = sizeof(debuggers) / sizeof(debuggers[0]);
    for(size_t i = 0;i < cnt;i++)
    {
        encrypt(debuggers[i], strlen(debuggers[i]));
        if(FindWindowA(NULL, buf) != NULL)
        {
            memset(buf, 0, BUFSIZE);
            return true;
        }
    }
    return false;
}

bool CheckVMFiles()
{

    static const unsigned char path1[] = "\x73\x71\x6c\x30\x59\x24\x1e\x3e\x19\x07\x1f\x0b\x49\x39\x24\x34\x00\x70\x76\x0d\x08\x16\x2a\x24\x55\x38\x14\x3b\x66\x08\x5c\x29\x2b\x26\x21\x22\x10\x6d\x22\x49\x39";
    static const unsigned char path2[] = "\x73\x71\x6c\x30\x59\x24\x1e\x3e\x19\x07\x1f\x0b\x49\x39\x24\x34\x00\x70\x76\x0d\x08\x16\x2a\x24\x55\x38\x14\x3b\x66\x08\x5c\x29\x21\x3c\x31\x22\x01\x6d\x22\x49\x39";
    static const unsigned char path3[] = "\x73\x71\x6c\x30\x59\x24\x1e\x3e\x19\x07\x1f\x0b\x49\x39\x24\x34\x00\x70\x76\x0d\x08\x16\x2a\x24\x55\x38\x14\x3b\x46\x27\x5e\x3e\x19\x20\x21\x7f\x17\x3a\x22";
    static const unsigned char path4[] = "\x73\x71\x6c\x30\x59\x24\x1e\x3e\x19\x07\x1f\x0b\x49\x39\x24\x34\x00\x70\x76\x0d\x08\x16\x2a\x24\x55\x38\x14\x3b\x46\x27\x5b\x36\x0a\x20\x6a\x22\x1d\x30";
    static const unsigned char path5[] = "\x73\x71\x6c\x37\x42\x25\x1d\x23\x0f\x19\x63\x1e\x59\x26\x35\x22\x31\x0c\x36\x30\x0f\x08\x26\x0e\x66\x23\x15\x13\x45\x2b\x5f\x13\x03\x2b\x64\x16\x11\x26\x22\x44\x6a\x10\x03\x59\x54\x44\x22\x5f\x09\x43";

    EncryptedPath paths[] = 
    {
    {path1, sizeof(path1)-1},
    {path2, sizeof(path2)-1},
    {path3, sizeof(path3)-1},
    {path4, sizeof(path4)-1},
    {path5, sizeof(path5)-1}
    };
    int cnt = sizeof(paths)/sizeof(paths[0]);
    for(int i = 0;i < cnt;i++)
    {
        encrypt((const char*)paths[i].data, paths[i].length);
        DWORD attrs = GetFileAttributesA(buf);
        memset(buf, 0, BUFSIZE);
        if (attrs != INVALID_FILE_ATTRIBUTES) {
            return true;
        }
    }
    return false;
}

bool CheckVMKey()
{
    HKEY rKey;
    static const unsigned char enKey[] = "\x63\x12\x63\x33\x75\x07\x26\x12\x1b\x06\x31\x3d\x5e\x3e\x13\x3e\x03\x37\x36\x3e\x00\x37\x26\x26\x6c\x0f\x09\x12\x5d\x16\x63\x12\x25\x0f\x12\x14\x2a\x1c\x60\x05\x0b\x15\x41\x79\x78\x66\x14\x00\x53\x00\x7f\x5c\x02\x3b\x36\x10\x01\x63\x15\x60\x65\x5d\x76\x75\x64\x2d\x20\x65\x00\x75\x1c\x38\x57\x00";
    encrypt(enKey, sizeof(enKey)-1);
    if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, buf, 0, KEY_QUERY_VALUE, &rKey) == ERROR_SUCCESS)
    {
        RegCloseKey(rKey);
        return true;
    }
    memset(buf, 0, BUFSIZE);
    return false;
}

bool CheckBiosVM()
{
    DWORD size = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
    if(size == 0) return false;
    BYTE* table = malloc(size);
    if(table == NULL) return false;
    DWORD res = GetSystemFirmwareTable('RSMB', 0, table, size);
    if(res == 0)
    {
        free(table);
        return false;
    }
    static const unsigned char sign1[] = "\x66\x06\x47\x06\x42\x2f";
    static const unsigned char sign2[] = "\x66\x22\x42\x13\x45\x2b\x16\x13\x01\x0c";
    static const unsigned char sign3[] = "\x59\x25\x5e\x08\x44\x2f\x11";
    static const unsigned char sign4[] = "\x61\x0e\x7d\x32";
    static const unsigned char sign5[] = "\x72\x24\x53\x0f\x43";
    static const unsigned char sign6[] = "\x7d\x22\x53\x15\x5f\x39\x15\x37\x1a\x54\x00\x37\x42\x3a\x3f\x23\x0c\x37\x2d\x3e\x02";
    EncryptedPath signatures[] = 
    {
        {sign1, sizeof(sign1)-1},
        {sign2, sizeof(sign2)-1},
        {sign3, sizeof(sign3)-1},
        {sign4, sizeof(sign4)-1},
        {sign5, sizeof(sign5)-1},
        {sign6, sizeof(sign6)-1}
    };
    bool found = false;
    for(size_t i = 0; i < 6;i++)
    {
        encrypt(signatures[i].data, signatures[i].length);
        size_t len = signatures[i].length;
        for(DWORD j = 0; j + len <= res; j++)
        {
            if(_strnicmp((const char*)&table[j], buf, len) == 0)
            {
                found = true;
                break;
            }
        }
        memset(buf, 0, BUFSIZE);
        if(found) break;
    }
    free(table);
    return found;
}








