#ifndef CPU_H
#define CPU_H
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "sha1.h"
// void generateSalt(char salt[]);
void DictBruteCPU(char salt[]);
#endif /*CPU_H*/