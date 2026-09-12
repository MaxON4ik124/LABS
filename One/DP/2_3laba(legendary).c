#include <stdio.h>
#include <math.h>
#include <stdlib.h>
int checkstepdown(int arr[11][13], int x, int y, int curline)
{
    if(x+1 < 11 && arr[x][y] <= arr[x+1][y])
        return checkstepdown(arr, x+1, y, curline+1);
    return curline;
}
int checkstepleft(int arr[11][13], int x, int y, int curline)
{
    if(x-1 > -1 && y-1 > -1 && arr[x][y] <= arr[x-1][y-1])
        return checkstepleft(arr, x-1, y-1, curline+1);
    return curline;
}
int checkstepright(int arr[11][13], int x, int y, int curline)
{
    if(x-1 > -1 && y+1 < 13 && arr[x][y] <= arr[x-1][y+1])
        return checkstepright(arr, x-1, y+1, curline+1);
    return curline;
}
int min(int a, int b, int c)
{
    int min;
    if(a < b)
        min = a;
    else
        min = b;
    if(min > c)
        min = c;
    // printf("%d %d %d %d\n", a, b, c, min);
    return min;
}
int findY(int arr[11][13], int x, int y, int curline)
{
    return min(checkstepdown(arr, x, y, curline), checkstepleft(arr, x, y, curline), checkstepright(arr, x, y, curline));

}
int main()
{
    int net[11][13];
    int maxsize = 1;
    int cursize;
    for(int i = 0;i < 11;i++)
    {
        for(int j = 0;j < 13;j++)
        {
            scanf("%d", &net[i][j]);
        }
    }
    
    for(int i = 0;i < 11;i++)
    {
        for(int j = 0;j < 13;j++)
        {
            cursize = findY(net, i, j, 1);
            if(cursize > maxsize)
                maxsize = cursize;
        }
    }
    
    // for(int i = 0;i < 5;i++)
    // {
    //     for(int j = 0;j < 5;j++)
    //     {
    //         printf("%d ", net[i][j]);
    //     }
    //     printf("\n");
    // }
    printf("%d", maxsize);

}