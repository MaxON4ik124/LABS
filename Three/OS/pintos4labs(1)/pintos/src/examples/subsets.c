#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>

void generate_subsets(int *subset, int start, int index, int N, int k);

void generate_subsets(int *subset, int start, int index, int N, int k) {
    if (index == k) {
        printf("{");
        for (int i = 0; i < k; i++) {
            printf("%d%s", subset[i], (i == k - 1) ? "" : ", ");
        }
        printf("}\n");
        return;
    }

    for (int i = start; i <= N; i++) {
        subset[index] = i;
        generate_subsets(subset, i + 1, index + 1, N, k);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    int k = 0;
    for (int i = 0; argv[1][i] != '\0'; i++) {
        k = k * 10 + (argv[1][i] - '0');
    }
    
    int N = 0;
    for (int i = 0; argv[2][i] != '\0'; i++) {
        N = N * 10 + (argv[2][i] - '0');
    }

    if (k <= 0 || N <= 0) {
        printf("Такое невозможно\n");
        return 1;
    }
    
    if (k > N) {
        printf("Такое невозможно\n");
        return 1;
    }
    
    int subset[k];
    generate_subsets(subset, 1, 0, N, k);
    
    return 0;
}

