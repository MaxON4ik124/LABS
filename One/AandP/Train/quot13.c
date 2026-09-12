/*
10.Функция, возвращающая i и j такие, чтобы сумма эл-тов в i-ой строке
равнялась сумме в j-ом столбце
*/
#include <stdio.h>
#define N 4
#define M 3
int a = 0;
int b = 0;
int matrix[N][M] = {{3, 2, 1}, {45, 12, 13}, {1, 45, 2}, {4, 2, 45}};
void eqsum()
{
    int col_sums[M] = {0};
    int row_sums[N] = {0};
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < M;j++)
        {
            col_sums[j] += matrix[i][j];
            row_sums[i] += matrix[i][j];
        }
    }
    for(int i = 0;i < M;i++)
    {
        for(int j = 0;j < M;j++)
        {
            if(row_sums[i] == col_sums[j])
            {
                a = i;
                b = j;
                return;
            }
        }
    }
    
}
int main()
{
    eqsum();
    printf("%d %d", a, b);
}