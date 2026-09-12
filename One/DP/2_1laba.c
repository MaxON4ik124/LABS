#include <stdio.h>
#include <math.h>
int main()
{
    int n;
    scanf("%d", &n);
    if(n <= 0)
    {
        printf("0");
        return 0;
    }
    
    int array[100000];
    double mid = 0;
    for(int i = 0;i < n;i++)
    {
        int el;
        scanf("%d", &el);
        array[i] = el;
        mid += el;
    }
    mid /= n;
    double minrange = 1488*1488;
    int nearest = 0;
    for(int i = 0; i < n;i++)
    {
        if (fabs((array[i] - mid)) < minrange)
        {
            nearest = array[i];
            minrange = fabs((array[i] - mid));
        }
    }
    printf("%d", nearest);
}