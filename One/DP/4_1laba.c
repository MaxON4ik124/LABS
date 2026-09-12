#include <stdio.h>
#include <math.h>
#include <stdlib.h>
int n, k, l;
int board[25][25] = {0};
int knights[25][2];
int vars[25][2];
int possible = 0;
int solve[1000][2];
int solves[20000][10000][2];
int totvars = 0;
int knight_take(int x1, int y1, int x2, int y2)
{
    if((abs(x1 - x2) == 2 && abs(y1 - y2) == 1) || abs(y1 - y2) == 2 && abs(x1 - x2) == 1)
        return 1;
    return 0;
}
int check_solve_for_takes(int arr[1000][2], int len)
{
    for(int i = 0;i < len;i++)
    {
        for(int j = i+1; j < len;j++)
        {
            if(knight_take(arr[i][0], arr[i][1], arr[j][0], arr[j][1]))
                return 0;
        }
    }
    return 1;
}
void analyze(int field[25][25], int len, int knight[2])
{
    for(int j = 0;j < len;j++)
        {
            for(int k = 0;k < len;k++)
            {
                if(knight[0] == j && knight[1] == k)
                    field[j][k] = 3;
                if(knight_take(knight[0], knight[1], j, k))
                    field[j][k] = 1;
            }
        }
}
void place_knights(int solve[1000][2], int start, int len)
{
    for(int i = start; i < possible;i++)
    {
        solve[len][0] = vars[i][0];
        solve[len][1] = vars[i][1];
        place_knights(solve, i + 1, len + 1);
    }
    if(len == l && check_solve_for_takes(solve, len))
    {
        for(int i = 0;i < k;i++)
        {
            solves[totvars][i][0] = knights[i][0];
            solves[totvars][i][1] = knights[i][1];
        }
        for(int i = 0;i < l;i++)
        {
            solves[totvars][i+k][0] = solve[i][0];
            solves[totvars][i+k][1] = solve[i][1];
        }
        totvars++;
    }
}
int main()
{
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");
    fscanf(input, "%d %d %d\n", &n, &l, &k);
    for(int i = 0;i < k;i++)
        fscanf(input, "%d %d\n", &knights[i][0], &knights[i][1]);
    for(int i = 0;i < k;i++)
        analyze(board, n, knights[i]);
    for(int x = 0;x < n;x++)
    {
        for(int y = 0;y < n;y++)
        {
            if(board[x][y] == 0)
            {
                vars[possible][0] = x;
                vars[possible][1] = y;
                possible++;
            }
        }
    }
    place_knights(solve, 0, 0);
    if(totvars == 0 || n == 0)
        fprintf(output, "no solutions");
    else
    {
        for(int i = 0;i < totvars;i++)
        {
            for(int j = 0;j < l+k;j++)
                fprintf(output, "(%d,%d) ", solves[i][j][0], solves[i][j][1]);
            if(i < totvars-1)
                fprintf(output, "\n");
        }}
    fclose(input);
    fclose(output);}