#include <stdio.h>
#include <cuda_runtime.h>
#include <string.h>
#include <time.h>
#include "sha1.cuh"
#include "GPU.h"

#define PASSWORD_MAX_LENGTH 64
#define MAX_PASSWORDS 1000000
#define NUM_STREAMS 8

// Кроссплатформенные макросы для больших файлов
#ifdef _WIN32
    #define FSEEK64 _fseeki64
    #define FTELL64 _ftelli64
    typedef __int64 file_offset_t;
#else
    #define FSEEK64 fseeko
    #define FTELL64 ftello
    typedef off_t file_offset_t;
#endif

__device__ __constant__ uint8_t d_targetHash[20];
__device__ __constant__ char    d_salt[64];
__device__ volatile int stopFlag = 0;

__device__ int gpu_strcspn(const char* str1, char ch)
{
    int i = 0;
    while(str1[i] != '\0' && str1[i] != ch)
        i++;
    return i;
}

__global__ void GPUCore(int cnt, char (*data)[PASSWORD_MAX_LENGTH])
{
    unsigned long long idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx >= cnt || stopFlag != 0)
        return;

    int f = 0;
    char* password = data[idx];
    
    uint8_t hash[20];
    gpu_sha1_hash((uint8_t*)password, (uint8_t*)d_salt, hash);
    f = 1;
    
    #pragma unroll
    for(int k = 0; k < 20; k++)
    {
        if(hash[k] != d_targetHash[k])
        {
            f = 0;
            break;
        }
    }
    
    if(f == 0)
        return;
    
    if(atomicExch((int*)&stopFlag, 1) == 0)
    {
        printf("Collision found: %s\n", password);
        printf("Used salt: %s\n", d_salt);
        printf("Hash: ");
        for (int i = 0; i < 20; i++)
            printf("%02x", hash[i]);
        printf("\n");
        return;
    }
}

extern "C" void DictBruteGPU(char salt[]) {
    char localSalt[512];
    strcpy(localSalt, salt);
    int totalCount = 0;
    clock_t start;
    double gpu_time_used;

    start = clock();
    
    FILE* passwords = fopen("passwds1.txt", "rb");  // Бинарный режим для больших файлов
    FILE* mainPass = fopen("mainPass.txt", "r");
    
    if(!passwords || !mainPass) {
        printf("Error opening files!\n");
        return;
    }
    
    uint8_t mainHash[20];
    char hexHash[41];
    
    fgets(hexHash, sizeof(hexHash), mainPass);
    fclose(mainPass);
    
    for (int i = 0; i < 20; i++)
        sscanf(&hexHash[i * 2], "%2hhx", &mainHash[i]);
    
    printf("Target hash & salt: ");
    for (int i = 0; i < 20; i++)
        printf("%02x", mainHash[i]);
    printf(" %s\n", localSalt);
    
    // ===== ИСПОЛЬЗОВАНИЕ FSEEKI64 ДЛЯ ПОЛУЧЕНИЯ РАЗМЕРА ФАЙЛА =====
    printf("\nDetermining file size using 64-bit seek...\n");
    
    FSEEK64(passwords, 0, SEEK_END);
    file_offset_t fileSize = FTELL64(passwords);
    FSEEK64(passwords, 0, SEEK_SET);
    
    if(fileSize < 0) {
        printf("Error: Failed to get file size!\n");
        fclose(passwords);
        return;
    }
    
    printf("File size: %lld bytes (%.2f GB)\n", 
           (long long)fileSize, 
           (double)fileSize / (1024.0 * 1024.0 * 1024.0));
    
    // ===== ЗАГРУЗКА ВСЕГО ФАЙЛА В ПАМЯТЬ =====
    printf("Loading entire file into memory...\n");
    
    char* fileData;
    cudaMallocHost((void**)&fileData, (size_t)fileSize + 1);
    
    size_t bytesRead = fread(fileData, 1, (size_t)fileSize, passwords);
    fileData[bytesRead] = '\0';
    fclose(passwords);
    
    printf("File loaded: %zu bytes (%.2f GB)\n", 
           bytesRead, 
           (double)bytesRead / (1024.0 * 1024.0 * 1024.0));
    
    printf("\nStarting dictionary brute-force on GPU...\n");
    
    // Сброс флага остановки
    int zero = 0;
    cudaMemcpyToSymbol(stopFlag, &zero, sizeof(int), 0, cudaMemcpyHostToDevice);
    
    // Копирование целевого хеша и соли в константную память
    cudaMemcpyToSymbol(d_targetHash, mainHash, 20 * sizeof(uint8_t));
    cudaMemcpyToSymbol(d_salt, localSalt, strlen(localSalt) + 1);
    
    // ===== СОЗДАНИЕ STREAMS =====
    cudaStream_t streams[NUM_STREAMS];
    for(int i = 0; i < NUM_STREAMS; i++) {
        cudaStreamCreate(&streams[i]);
    }
    
    // ===== ВЫДЕЛЕНИЕ PINNED MEMORY ДЛЯ КАЖДОГО STREAM =====
    char (*h_data[NUM_STREAMS])[PASSWORD_MAX_LENGTH];
    for(int i = 0; i < NUM_STREAMS; i++) {
        cudaMallocHost((void**)&h_data[i], MAX_PASSWORDS * PASSWORD_MAX_LENGTH * sizeof(char));
    }
    
    // ===== ВЫДЕЛЕНИЕ DEVICE MEMORY ДЛЯ КАЖДОГО STREAM =====
    char (*d_data[NUM_STREAMS])[PASSWORD_MAX_LENGTH];
    for(int i = 0; i < NUM_STREAMS; i++) {
        cudaMalloc((void**)&d_data[i], MAX_PASSWORDS * PASSWORD_MAX_LENGTH * sizeof(char));
    }
    
    int threadsPerBlock = 256;
    int streamIdx = 0;
    int activeBatches = 0;
    
    // ===== ПАРСИНГ ИЗ ПАМЯТИ (НЕТ DISK I/O) =====
    char* currentPos = fileData;
    char* endPos = fileData + bytesRead;
    
    while(currentPos < endPos)
    {
        int localcnt = 0;
        
        // Читаем батч паролей из памяти
        while(localcnt < MAX_PASSWORDS && currentPos < endPos)
        {
            char* lineEnd = strchr(currentPos, '\n');
            if(lineEnd == NULL) lineEnd = endPos;
            
            int len = lineEnd - currentPos;
            
            // Пропускаем пустые строки
            if(len > 0 && len < PASSWORD_MAX_LENGTH)
            {
                memcpy(h_data[streamIdx][localcnt], currentPos, len);
                h_data[streamIdx][localcnt][len] = '\0';
                localcnt++;
            }
            
            currentPos = (lineEnd < endPos) ? lineEnd + 1 : endPos;
        }
        
        if(localcnt == 0)
            break;
        
        totalCount += localcnt;
        
        // Асинхронное копирование данных на GPU
        cudaMemcpyAsync(d_data[streamIdx], h_data[streamIdx], 
                       localcnt * PASSWORD_MAX_LENGTH * sizeof(char), 
                       cudaMemcpyHostToDevice, streams[streamIdx]);
        
        // Запуск ядра в том же stream
        int blocksPerGrid = (localcnt + threadsPerBlock - 1) / threadsPerBlock;
        GPUCore<<<blocksPerGrid, threadsPerBlock, 0, streams[streamIdx]>>>(localcnt, d_data[streamIdx]);
        
        activeBatches++;
        
        // // Вывод прогресса каждые 1M паролей
        // if(totalCount % 1000000 < MAX_PASSWORDS) {
        //     double elapsed = ((double)(clock() - start)) / CLOCKS_PER_SEC;
        //     printf("Processed: %d passwords (%.2f M/s)         \r", 
        //            totalCount, 
        //            totalCount / elapsed / 1000000.0);
        //     fflush(stdout);
        // }
        
        // Переключение на следующий stream
        streamIdx = (streamIdx + 1) % NUM_STREAMS;
        
        // Синхронизация только если все streams заняты
        if(activeBatches >= NUM_STREAMS) {
            cudaStreamSynchronize(streams[streamIdx]);
            activeBatches--;
        }
        
        // Проверка флага остановки
        int h_flag = 0;
        cudaMemcpyFromSymbol(&h_flag, stopFlag, sizeof(int));
        
        if (h_flag != 0)
        {
            printf("\nPassword found! Stopping search.\n");
            break;
        }
    }
    
    // ===== ОЖИДАНИЕ ЗАВЕРШЕНИЯ ВСЕХ STREAMS =====
    for(int i = 0; i < NUM_STREAMS; i++) {
        cudaStreamSynchronize(streams[i]);
    }
    
    // ===== ОСВОБОЖДЕНИЕ ПАМЯТИ =====
    cudaFreeHost(fileData);
    
    for(int i = 0; i < NUM_STREAMS; i++) {
        cudaFreeHost(h_data[i]);
        cudaFree(d_data[i]);
        cudaStreamDestroy(streams[i]);
    }
    
    gpu_time_used = ((double)(clock() - start)) / CLOCKS_PER_SEC;
    printf("\n\nGPU Time used: %.3f s.\n", gpu_time_used);
    printf("Total passwords processed: %d\n", totalCount);
    printf("Average speed: %.2f M passwords/sec\n", totalCount / gpu_time_used / 1000000.0);
}
