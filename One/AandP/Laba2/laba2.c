#include <stdio.h>
#include <math.h>
#include <float.h>
// #include <gmp.h>
#define PI 3.14159
long double fact(unsigned int x)
{
    if(x < 2)
        return x;
    return x * fact(x-1);
}
long double power(long double base, unsigned int ind)
{
    long double res = 1;
    for(int i = 0;i < ind; i++)
        res *= base;
    return res;
}
long double f(long double x, unsigned int n, int mode)
{
    long double res = 0;
    if(mode == 0)
    {
        for(int i = 1; i <= n;i++)
        {
            res += power(x, 2*i-1)/fact(2*i-1) * power(-1.0, i+1);
        }
        return res;
    }
    else if(mode > 0)
    {
        while (x > 2*PI)
            x -= 2*PI;
        int g = 2;
        while(fabsl(f(x, g, 0) - f(x, g-1, 0)) > LDBL_EPSILON)
            g++;
        if(mode == 1)
            return f(x, g, 0);
        if(mode == 2)
            return g;
    }
}
int main()
{
    unsigned int n;
    long double x;
    printf("Enter x: ");
    scanf("%Lf", &x);
    printf("Enter n: ");
    scanf("%u", &n);
    printf("\nResult: %.30Lf (accuracy = %u)",f(x, n, 0), n);
    printf("\nResult: %.30Lf (accuracy => inf)",f(x, n, 1));
    printf("\nCount of members: %.0Lf", f(x, n, 2));
    printf("\nResult: %.30Lf (sin(%.2Lf))", sinl(x), x);
    return 0;
}