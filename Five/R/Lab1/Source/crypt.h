#ifndef CRYPT_H
#define CRYPT_H
#define BUFSIZE 200
extern char buf[200];
#include "main.h"

typedef struct {
    const unsigned char* data;
    size_t length;
} EncryptedPath;

void encrypt(const char* source, size_t size);
bool CheckPass(char* PwdInp);
bool isPrime(int num);
uint32_t calcCRC(const char* filename);
bool checkCRC();
void serial();
bool checkPass(char* PwdInp);
bool Checkpass(char* PwdInp);
bool checkpass(char* PwdInp);
bool check_pass(char* PwdInp);
bool Check_Pass(char* PwdInp);
bool check_Pass(char* PwdInp);
bool Check_pass(char* PwdInp);
bool CheckIDP();
bool CheckRemoteDebug();
bool CheckTicks();
bool CheckNtGlobalFlag();
bool CheckWindows();
bool CheckVMFiles();
bool CheckVMKey();
bool CheckBiosVM();
#endif