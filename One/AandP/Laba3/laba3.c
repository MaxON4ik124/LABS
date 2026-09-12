#include <stdio.h>
#include <string.h>
#include <math.h>
#define SIZE 10000
#define ACCURACY 2
struct CFloat
{
    short mantissa[SIZE];
    short exponent;
    short sign;
};
struct CFloat toZero()
{
    struct CFloat result;
    result.exponent = 1;
    result.sign = 0;
    return result;
}
struct CFloat minus(struct CFloat val1, struct CFloat val2);
int max(int a, int b)y
{
    return (a > b) ? a : b;
}
int get_size(struct CFloat val)
{
    int size = SIZE - 1;
    while(size >= val.exponent && val.mantissa[size] == 0)
        size--;
    return size + 1;
}
int equal(struct CFloat a, struct CFloat b)
{
    for(int i = 0; i < SIZE; i++)
    {
        if(a.mantissa[i] != b.mantissa[i])
            return 0;
    }
    return 1;
}
struct CFloat compare(struct CFloat val1, struct CFloat val2, int arg)
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
int higher(struct CFloat val1, struct CFloat val2)
{
    if(equal(compare(val1, val2, 1), val1))
        return 1;
    return 0;
}
struct CFloat offset(struct CFloat val, int bias, int side)
{
    struct CFloat result;
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
struct CFloat convert_toCF(long double val)
{
    struct CFloat result;
    for(int i = 0;i < SIZE;i++) result.mantissa[i] = 0;
    char transfer[SIZE];
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
            result.mantissa[i - minus - dot] = transfer[i] - '0';
    }
    return result;
}
void print_cfloat(struct CFloat cf)
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
struct CFloat get_cfloat()
{
    struct CFloat minimum;
    struct CFloat result;
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
struct CFloat plus(struct CFloat val1, struct CFloat val2)
{
    struct CFloat result;
    for(int i = 0; i < SIZE; i++) 
        result.mantissa[i] = 0;
    
    result.exponent = max(val1.exponent, val2.exponent);
    struct CFloat part1 = offset(val1, result.exponent-val1.exponent+1, 1);
    struct CFloat part2 = offset(val2, result.exponent-val2.exponent+1, 1);
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
        struct CFloat p1 = val1;
        struct CFloat p2 = val2;
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
        struct CFloat p1 = val1;
        struct CFloat p2 = val2;
        p1.sign = 0;
        p2.sign = 0;
        result = minus(p1, p2);
        return result;
    }
}
struct CFloat minus(struct CFloat val1, struct CFloat val2)

{
    struct CFloat result;
    for(int i = 0; i < SIZE; i++) 
        result.mantissa[i] = 0;
    result.sign = 0;
    result.exponent = max(val1.exponent, val2.exponent);
    struct CFloat part1 = offset(val1, result.exponent-val1.exponent, 1);
    struct CFloat part2 = offset(val2, result.exponent-val2.exponent, 1);
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
        if(equal(val1, val2))
        {
            result = toZero();
            return result;
        }
    }
    if(val1.sign == 1 && val2.sign == 0)
    {
        if(equal(val1, val2))
        {
            result = toZero();
            return result;
        }
        struct CFloat p1 = val1;
        struct CFloat p2 = val2;
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
        struct CFloat p1 = val1;
        struct CFloat p2 = val2;
        p1.sign = 0;
        p2.sign = 0;
        result = plus(p1, p2);
        result.sign = 0;
        return result;
    }
    if(val1.sign == 1 && val2.sign == 1)
    {
        if(equal(val1, val2))
        {
            result = toZero();
            return result;
        }
        struct CFloat p1 = val1;
        struct CFloat p2 = val2;
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
    if(equal(val1, val2) && val1.sign == 0 && val2.sign == 0)
        result = toZero();
    return result;
    
}
struct CFloat multiply(struct CFloat val1, struct CFloat val2)
{
    struct CFloat result;
    for(int i = 0;i < SIZE-1;i++) result.mantissa[i] = 0;
    result.sign = val1.sign ^ val2.sign;
    int acc1 = get_size(val1) - val1.exponent;
    int acc2 = get_size(val2) - val2.exponent;
    int len1 = get_size(val1);
    int len2 = get_size(val2);
    struct CFloat fact1;
    struct CFloat fact2;
    for(int i = 0;i < SIZE-1;i++) fact1.mantissa[i] = 0;
    for(int i = 0;i < SIZE-1;i++) fact2.mantissa[i] = 0;
    fact1 = offset(val1, 1, 1);
    fact2 = offset(val2, 1, 1);
    for(int i = len1;i > 0;i--)
    {
        for(int j = len2;j > 0;j--)
        {
            if(i + j >= SIZE-1) continue;
            else
                result.mantissa[i + j - 1] += fact1.mantissa[i] * fact2.mantissa[j];
        }
    }
    for(int i = len2+len1-1;i > 0;i--)
    {
        if(result.mantissa[i] >= 10)
        {
            result.mantissa[i-1] += result.mantissa[i] / 10;
            result.mantissa[i] %= 10;
        }
    }
    int len = len1 + len2 - 1;
    if(result.mantissa[0] == 0)
        result = offset(result, 1, -1);
    else
        len++;
    result.exponent = len - acc1 - acc2;
    return result;
}
struct CFloat raise(struct CFloat base, int index)
{
    struct CFloat result = convert_toCF(1.0);
    for(int i = 0; i < index;i++)
        result = multiply(result, base);
    return result;
}
struct CFloat factorial(int n)
{
    struct CFloat result = convert_toCF(1.0);
    for(int i = 1;i <= n;i++)
        result = multiply(result, convert_toCF(i));
    return result;
}
struct CFloat totalsum(int a, int b)
{
    struct CFloat result = convert_toCF(a);
    for(int i = a+1;i <= b;i++)
        result = plus(result, convert_toCF(i));
    return result;
}
int main()
{
    char op[1];
    int fuse = 0;
    struct CFloat val = convert_toCF(0.0);
    printf("Enter number 1: ");
    struct CFloat val1 = get_cfloat();
    printf("Enter number 2: ");
    struct CFloat val2 = get_cfloat();
    printf("Enter operator (+ for plus;- for minus; * for multiply; ^ for power; ! for factorial; & for sum from X to Y; 0 for exit): ");
    scanf("%s", op);
    while(op[0] != '0')
    {
        if(op[0] == '+')
        {
            val = plus(val1, val2);
            printf("Result: ");
            print_cfloat(val);
            fuse = 1;
        }
        else if(op[0] == '-')
        {
            val = minus(val1, val2);
            printf("Result: ");
            print_cfloat(val);
            fuse = 1;
        }
        else if(op[0] == '*')
        {
            val = multiply(val1, val2);
            printf("Result: ");
            print_cfloat(val);
            fuse = 1;
        }
        else if(op[0] == '^')
        {
            int ind;
            printf("Enter base: ");
            val1 = get_cfloat();
            printf("Enter index: ");
            scanf("%d", &ind);
            val = raise(val1, ind);
            printf("Result: ");
            print_cfloat(val);
            fuse = 1; 
        }
        else if(op[0] == '!')
        {
            int n;
            printf("Enter indicator: ");
            scanf("%d", &n);
            val = factorial(n);
            printf("Result: ");
            print_cfloat(factorial(n));
            fuse = 1; 
        }
        else if(op[0] == '&')
        {
            int x;
            int y;
            printf("Enter the 1st number: ");
            scanf("%d", &x);
            printf("Enter last number: ");
            scanf("%d", &y);
            val = totalsum(x, y);
            printf("Result: ");
            print_cfloat(val);
            fuse = 1; 
        }
        else if(op[0] == '0')
            break;
        if(fuse == 1)
        {
            printf("Continue with existing value? (y - yes; n - not; 0 - exit): ");
            scanf("%s", op);  
        }
        if(op[0] == 'y')
        {
            val1 = val;
            printf("Enter number 2: ");
            val2 = get_cfloat();
            fuse = 0;
        }
        else if(op[0] == 'n')
        {
            val = convert_toCF(0);
            printf("Enter number 1: ");
            val1 = get_cfloat();
            printf("Enter number 2: ");
            val2 = get_cfloat();
            fuse = 0;
        }
        else if(op[0] == '0')
            break;
        if(fuse == 0)
        {
            printf("Enter operator (+ for plus;- for minus; * for multiply; ^ for power; ! for factorial; & for sum from X to Y; 0 for exit): ");
            scanf("%s", op);
        }
        
    }
}
