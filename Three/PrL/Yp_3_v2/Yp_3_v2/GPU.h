#pragma once
#include <stdint.h>
#define PASSWORD_MAX_LENGTH 64

#ifdef __cplusplus
extern "C" 
{
#endif
void DictBruteGPU(char salt[]);
#ifdef __cplusplus
}
#endif /*GPU_H*/