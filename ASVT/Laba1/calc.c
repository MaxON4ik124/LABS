#include <stdio.h>

int main(void)
{
    int F_CPU = 8000000;
    int N_REQUIRED = 80000;

    int x, y;
    int N;

    for (y = 0; y <= 255; y++) {
        for (x = 0; x <= 255; x++) {


            N = 771 * y - 3 * x + 5;

            if (N == N_REQUIRED) {
                printf("Found exact match:\n");
                printf("x = %d, y = %d\n", x, y);
                printf("N = %d cycles\n", N);
                printf("t = %.6f s\n", (double)N / F_CPU);
                return 0;
            }
        }
    }

    printf("No exact solution found.\n");
    return 0;
}
