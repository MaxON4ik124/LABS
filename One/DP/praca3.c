#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define N 6
#define M 5

int main()
{
    int i, j;
    int matrix[N][M] = {0};
    int count = 1;
    for(j = 0; j < M-1; j++)
    {
        matrix[0][j] = count;
        count++;
    }
    for(i = 0; i < N-1; i++)
    {
        matrix[i][M-1] = count;
        count++;
    }
    for(i = N-1; i > 0; i--)
    {
        matrix[N-1][i] = count;
        count++;
    }
    for(j = M; j > 0; j--)
    {
        matrix[j][0] = count;
        count++;
    }

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
    
}