#include <stdio.h>
#include <string.h>
#define SIZE 3000
#define ACCURACY 30
typedef struct
{
    short mantissa[SIZE];
    short exponent;
    short sign;
} CFloat;
CFloat minus(CFloat val1, CFloat val2);
CFloat CreatePi()
{
    char Pi[100] = "3141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067";
    CFloat result;
    for(int i = 0;i < 100;i++)
        result.mantissa[i] = Pi[i] - '0';
    result.exponent = 1;
    result.sign = 0;
    return result;
}
CFloat toZero()
{
    CFloat result;
    result.exponent = 1;
    result.sign = 0;
    for(int i = 0;i < SIZE;i++) result.mantissa[i] = 0;
    return result;
}
CFloat createEpsilon()
{
    CFloat result;
    for(int i = 0;i < SIZE;i++) result.mantissa[i] = 0;
    result.mantissa[ACCURACY] = 1;
    result.exponent = 1;
    result.sign = 0;
    return result;
}
int max(int a, int b)
{
    return (a > b) ? a : b;
}
int get_size(CFloat val)
{
    int size = SIZE - 1;
    while(size >= val.exponent && val.mantissa[size] == 0)
        size--;
    return size + 1;
}
int equal(CFloat a, CFloat b)
{
    if(a.exponent != b.exponent)
        return 0;
    for(int i = 0; i < SIZE; i++)
    {
        if(a.mantissa[i]!=b.mantissa[i])
            return 0;
    }
    return 1;
}
CFloat compare(CFloat val1, CFloat val2, int arg)
{
    if(val1.exponent > val2.exponent && arg == 1)
        return val1;
    if(val1.exponent < val2.exponent && arg == 1)
        return val2;
    if(val1.exponent > val2.exponent && arg == 0)
        return val2;
    if(val1.exponent < val2.exponent && arg == 0)
        return val1;
    for(int i = 0; i < SIZE; i++)
    {
        if(val1.mantissa[i] > val2.mantissa[i] && arg == 1)
            return val1;
        if(val1.mantissa[i] < val2.mantissa[i] && arg == 1)
            return val2;
        if(val1.mantissa[i] > val2.mantissa[i] && arg == 0)
            return val2;
        if(val1.mantissa[i] < val2.mantissa[i] && arg == 0)
            return val1;
    }
    return val1;
}
int higher(CFloat val1, CFloat val2)
{
    if(equal(compare(val1, val2, 1), val1))
        return 1;
    return 0;
}
CFloat offset(CFloat val, int bias, int side)
{
    CFloat result;
    result.sign = val.sign;
    if(side == 1)
    {
        for(int i = 0; i < SIZE-bias; i++)
        {
            result.mantissa[i + bias] = val.mantissa[i];
        }
        for(int i = 0;i < bias;i++)
        {
            result.mantissa[i] = 0;
        }
        result.exponent = val.exponent + bias;
    }
    if(side == -1)
    {
        for(int i = 0; i < SIZE-bias; i++)
            result.mantissa[i] = val.mantissa[i + bias];
        result.exponent = val.exponent - bias;
    }
    if(result.exponent < 1)
        result.exponent = 1;
    return result;
}
CFloat convert_toCF(long double val)
{
    CFloat result;
    for(int i = 0;i < SIZE;i++) result.mantissa[i] = 0;
    char transfer[SIZE] = {'\0'};
    result.sign = 0;
    snprintf(transfer, SIZE, "%Lf", val);
    int minus = 0;
    int dot = 0;
    int i = 0;
    if(transfer[0] == '-')
    {
        result.sign = 1;
        minus = 1;
        i++;
    }
    for(i; i < SIZE; i++)
    {
        if(transfer[i] == '.')
        {
            dot = 1;
            result.exponent = i - minus;
        }
        else
        {
            if(transfer[i] >= '0' && transfer[i] <= '9')
                result.mantissa[i - minus - dot] = transfer[i] - '0';
            else
                result.mantissa[i - minus - dot] = 0;
        }
    }
    return result;
}
void print_cfloat(CFloat cf)
{
    if(cf.sign == 1)
        printf("-");
    
    for(int i = 0;i < ACCURACY+cf.exponent; i++)
    {
        if(i == cf.exponent) printf(".");
        printf("%d", cf.mantissa[i]);
    }
    printf("\n");
}
CFloat get_cfloat()
{
    // CFloat minimum;
    CFloat result;
    result.sign = 0;
    result.exponent = 0;
    for(int i = 0; i < SIZE; i++) 
        result.mantissa[i] = 0;
    char number[SIZE + 1];
    scanf("%s", number);
    int minus = 0;
    int len = strlen(number) - 1;
    int dot = 0;
    int i = 0;
    if(number[0] == '-')
    {
        result.sign = 1;
        minus = 1;
        i = 1;
    }
    for(i; i <= len; i++)
    {
        if(number[i] == '.')
        {
            dot = 1;
            result.exponent = i - minus;
        }
        else
            result.mantissa[i - minus - dot] = number[i] - '0';
    }
    if(dot == 0)
        result.exponent = len - minus + 1;
    return result;
}
CFloat plus(CFloat val1, CFloat val2)
{
    CFloat result;
    for(int i = 0; i < SIZE; i++) 
        result.mantissa[i] = 0;
    
    result.exponent = max(val1.exponent, val2.exponent);
    CFloat part1 = offset(val1, result.exponent-val1.exponent+1, 1);
    CFloat part2 = offset(val2, result.exponent-val2.exponent+1, 1);
    result.exponent++;
    if(val1.sign == val2.sign)
    {
        result.sign = val1.sign;
        for(int i = SIZE-1; i > 0; i--)
        {
            result.mantissa[i] = part1.mantissa[i] + part2.mantissa[i];
        }
        for(int i = SIZE-1; i > 0; i--)
        {
            if(result.mantissa[i] >= 10)
            {
                result.mantissa[i] -= 10;
                result.mantissa[i-1]++;
            }
        }
        if(result.mantissa[0] == 0)
            result = offset(result, 1, -1);
        return result;
    }
    if(val1.sign == 1 && val2.sign == 0)
    {
        if(equal(val1, val2))
        {
            result = toZero();
            return result;
        }
        CFloat p1 = val1;
        CFloat p2 = val2;
        p1.sign = 0;
        p2.sign = 0;
        result = minus(p2, p1);
        return result;
    }
    if(val1.sign == 0 && val2.sign == 1)
    {
        if(equal(val1, val2))
        {
            result = toZero();
            return result;
        }
        CFloat p1 = val1;
        CFloat p2 = val2;
        p1.sign = 0;
        p2.sign = 0;
        result = minus(p1, p2);
        return result;
    }
}
CFloat minus(CFloat val1, CFloat val2)

{
    CFloat result;
    for(int i = 0; i < SIZE; i++) 
        result.mantissa[i] = 0;
    result.sign = 0;
    result.exponent = max(val1.exponent, val2.exponent);
    CFloat part1 = offset(val1, result.exponent-val1.exponent, 1);
    CFloat part2 = offset(val2, result.exponent-val2.exponent, 1);
    if(equal(compare(val1, val2, 1), val2) == 1 && val1.sign == 0 && val2.sign == 0)
    {
        if(equal(val1, val2))
        {
            result = toZero();
            return result;
        }
        result = minus(val2, val1);
        result.sign = 1;
        return result;
    }
    if(val1.sign == 0 && val2.sign == 0)
    {
        for(int i = SIZE-1; i > -1; i--)
        {
            result.mantissa[i] = part1.mantissa[i] - part2.mantissa[i];
        }
        for(int i = SIZE-1; i > -1; i--)
        {
            if(result.mantissa[i] < 0)
            {
                result.mantissa[i] += 10;
                result.mantissa[i-1]--;
            }
        }
    }
    if(val1.sign == 1 && val2.sign == 0)
    {
        if(equal(val1, val2))
        {
            result = toZero();
            return result;
        }
        CFloat p1 = val1;
        CFloat p2 = val2;
        p1.sign = 0;
        p2.sign = 0;
        result = plus(p1, p2);
        result.sign = 1;
        return result;
    }
    if(val1.sign == 0 && val2.sign == 1)
    {
        if(equal(val1, val2))
        {
            result = toZero();
            return result;
        }
        CFloat p1 = val1;
        CFloat p2 = val2;
        p1.sign = 0;
        p2.sign = 0;
        result = plus(p1, p2);
        result.sign = 0;
        return result;
    }
    if(val1.sign == 1 && val2.sign == 1)
    {
        CFloat p1 = val1;
        CFloat p2 = val2;
        p1.sign = 0;
        p2.sign = 0;
        result = minus(p1, p2);
        result.sign = 0;
        return result;
    }
    int pref = 0;
    while(result.mantissa[pref] == 0 && pref < result.exponent-1)
        pref++;
    result = offset(result, pref, -1);
    return result;
    
}
CFloat multiply(CFloat val1, CFloat val2)
{
    CFloat Zero = toZero();
    CFloat result;
    if(equal(val1, Zero) || equal(val2, Zero))
        return Zero;
    for(int i = 0;i < SIZE;i++) result.mantissa[i] = 0;
    int acc1 = get_size(val1) - val1.exponent;
    int acc2 = get_size(val2) - val2.exponent;
    int len1 = get_size(val1);
    int len2 = get_size(val2);
    CFloat fact1;
    CFloat fact2;
    for(int i = 0;i < SIZE;i++) fact1.mantissa[i] = 0;
    for(int i = 0;i < SIZE;i++) fact2.mantissa[i] = 0;
    fact1 = offset(val1, 1, 1);
    fact2 = offset(val2, 1, 1);
    int len = len1 + len2 - 1;
    for(int i = len1;i > 0;i--)
    {
        for(int j = len2;j > 0;j--)
        {
            result.mantissa[i + j - 1] += fact1.mantissa[i] * fact2.mantissa[j];
        }
        for(int i = len;i > 0;i--)
        {
            if(result.mantissa[i] >= 10)
            {
                result.mantissa[i-1] += result.mantissa[i] / 10;
                result.mantissa[i] = result.mantissa[i] % 10;
            }
        }
    }
    for(int i = len;i > 0;i--)
    {
        if(result.mantissa[i] >= 10)
        {
            result.mantissa[i-1] += result.mantissa[i] / 10;
            result.mantissa[i] = result.mantissa[i] % 10;
        }
    }
    if(result.mantissa[0] == 0)
        result = offset(result, 1, -1);
    else
        len++;
    for(int i = len;i < SIZE;i++) result.mantissa[i] = 0;
    result.exponent = len - acc1 - acc2;
    if(val1.sign == val2.sign)
        result.sign = 0;
    else
        result.sign = 1;
    return result;
}
CFloat raise(CFloat base, int index)
{
    CFloat result = convert_toCF(1.0);
    for(int i = 0; i < index;i++)
        result = multiply(result, base);
    return result;
}
CFloat factorial(int n)
{
    CFloat result = convert_toCF(1);
    for(int i = 1;i <= n;i++)
        result = multiply(result, convert_toCF(i));
    return result;
}
CFloat subCFloat(CFloat val, int begin, int end)
{
    CFloat result;
    for(int i = 0; i < SIZE; i++) result.mantissa[i] = 0;
    for(int i = 0;i <= end-begin; i++)
        result.mantissa[i] = val.mantissa[begin + i];
    result.exponent = end-begin + 1;
    result.sign = 0;
    return result;
}
CFloat divide(CFloat val1, CFloat val2)
{
    CFloat remainder = toZero();
    CFloat result = toZero();
    CFloat dividend = val1;
    CFloat divisor = val2;
    divisor.exponent = get_size(val2);
    dividend.exponent = val1.exponent + (divisor.exponent - val2.exponent);
    int resind = 0;
    int zind = 0;
    int prepare = 1;
    int specoffs = 1;
    int leadzeros = 0;
    int finddiff = 0;
    while(dividend.mantissa[leadzeros] == 0) leadzeros++;
    while(dividend.mantissa[leadzeros + finddiff] == divisor.mantissa[finddiff]) 
        finddiff++;
    if(dividend.mantissa[leadzeros + finddiff] >= divisor.mantissa[finddiff])
        specoffs = 0;
    for(int i = 0;i < SIZE;i++)
    {
        remainder = multiply(remainder, convert_toCF(10.0));
        remainder = plus(remainder, subCFloat(val1, i, i));
        if(i == dividend.exponent)
            result.exponent = resind;
        if(higher(remainder, divisor))
        {
            zind = 0;
            prepare = 0;
            while(higher(remainder, divisor))
            {
                remainder = minus(remainder, divisor);
                result.mantissa[resind]++;
            }
            resind++;
        }
        else
        {
            if(prepare <= 0)
                zind++;
            if(zind > 0)
            {
                result.mantissa[resind] = 0;
                resind++;
            }
        }
    }
    if(higher(divisor, dividend))
    {
        result = offset(result, divisor.exponent-dividend.exponent+specoffs+leadzeros, 1);
        result.exponent = 1;
    }
    if(val1.sign == val2.sign)
        result.sign = 0;
    else
        result.sign = 1;
    return result;
}
CFloat TaylorMemb(CFloat val, int n)
{
    CFloat res = val;
    res = raise(res, 2*n-1);
    res = divide(res, factorial(2*n-1));
    res = multiply(res, raise(convert_toCF(-1), n+1));
    return res;
}
CFloat TaylorN(CFloat x, int n)
{
    CFloat res = x;
    for(int i = 2; i <= n;i++)
    {
        res = plus(res, TaylorMemb(x, i));
        printf("%d\n", i);
    }
    return res;
}
CFloat TaylorMax(CFloat x)
{
    CFloat pi = CreatePi();
    CFloat Dpi = multiply(convert_toCF(2.0), pi);
    CFloat minVal = createEpsilon();
    CFloat res;
    int priv = 0;
    int mins = 0;
    while(x.sign == 1)
        x = plus(x, Dpi);
    while(higher(x, Dpi))
        x = minus(x, Dpi);
    if(higher(x, pi))
    {
        x = minus(x, pi);
        priv = 1;
    }
    res = x;
    int n = 2;
    CFloat memb = x;
    while(higher(memb, minVal))
    {
        memb = TaylorMemb(x, n);
        res = plus(res, memb);
        n++;

    }
    printf("Total count of members: %d\n", n);
    if(priv)
        res.sign = 1;
    return res;
}
int main()
{
    printf("Enter x: ");
    CFloat x = get_cfloat();
    int n;
    printf("Enter n: ");
    scanf("%d", &n);
    CFloat res = TaylorN(x, n);
    printf("Result with %d members: ", n);
    print_cfloat(res);
    res = TaylorMax(x);
    printf("Result with max accuracy: ");
    print_cfloat(res);
}