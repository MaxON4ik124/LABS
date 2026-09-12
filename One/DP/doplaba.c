#include <stdio.h>
#include <math.h>


#define ACCURACY 0.001
double ifsqrt(unsigned long long r)
{
    long double l = 1.0;
	while (fabs(r / l - l) > ACCURACY)
	{
        // r = l * l
        // r < l * l
        // r > l * l
		l = (l + r / l) / 2.0;
	}
	unsigned long long res = l;
    return (res * res == r) ? 1 : 0;
}


int main()
{
    int n;
    scanf("%d", &n);
    if (n <= 0)
    {
        printf("-1");
        return 0;
    }
    
    int maxindex = 0;
    unsigned long long max = 0;
    int ifzero = 0;
    int zeroind = 0;
    for(int i = 0; i < n;i++)
    {
        unsigned long long iter;
        scanf("%llu", &iter);
        if(iter == 0 && ifzero == 0)
        {
            ifzero = 1;
            zeroind = i;
        }
        if(ifsqrt(iter) && iter > max)
        {
            max = iter;
            maxindex = i;
        }
        
    }
    if (max == 0 && ifzero == 1)
    {
        printf("%d", zeroind);
        return 0;
    }
    else if (max == 0 && ifzero == 0)
    {
        printf("-1");
        return 0;
    }
    printf("%d", maxindex);
}