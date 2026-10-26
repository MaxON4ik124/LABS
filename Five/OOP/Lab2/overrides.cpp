#include <iostream>



class Interface
{
public:
    virtual int function1(int x) = 0;
    virtual int function2(int x, int y) = 0;
    virtual int function3() = 0;

    virtual ~Interface() = default;
};

class ParrentClass : public Interface
{
protected:
    int a;
    int b;
    int c;
    int d;
public:
    ParrentClass(int a, int b, int c, int d) : a(a), b(b), c(c), d(d) {}
    
    virtual int function1(int x)
    {
        int t1 = a * b - c;
        int t2 = (t1 * x) % d;
        return t1 + t2 - d;
    };
    virtual int function2(int x, int y)
    {
        int t1 = y;
        int t2 = a - b + c;

        if(t1 < 0) t1 = -t1;

        for(int i = 0;i < t1;i++)
            t2 += (c * a) % x;
           
        return t2;
    }
    virtual int function3() = 0;

    virtual ~ParrentClass() = default;
};

// int ParrentClass::function3()
// {
//     return 0;
// }



class OverrideClass1 : public ParrentClass
{
public:
    OverrideClass1(int a, int b, int c, int d) : ParrentClass(a, b, c, d) {}
    
    int function1(int x) override
    {   
        int t1 = a + b + c + d;
        int t2 = t1 % x;
        return t2;
    }

    int function3() override
    {
        return a * b + c * d;
    }
};

class OverrideClass2 : public ParrentClass
{
public:
    OverrideClass2(int a, int b, int c, int d) : ParrentClass(a, b, c, d) {}

    int function2(int x, int y) override
    {
        return x + y;
    }

    int function3() override
    {
        return a * b + c * d;
    }
};

class OverrideClass3 : public ParrentClass
{
public:
    OverrideClass3(int a, int b, int c, int d) : ParrentClass(a, b, c, d) {}

    int function3() override
    {
        return a * b + c * d;
    }
};

int main()
{
    OverrideClass1 ovr1(13, 11, 29, 23);
    OverrideClass2 ovr2(13, 11, 29, 23);
    OverrideClass3 ovr3(13, 11, 29, 23);
    std::cout << "============================OVERRIDE 1=============================================" << std::endl;
    std::cout << ovr1.function1(10) << "\t" << ovr1.function2(12, 5) << "\t" << ovr1.function3() << std::endl;
    std::cout << "============================OVERRIDE 2=============================================" << std::endl;
    std::cout << ovr2.function1(10) << "\t" << ovr2.function2(12, 5) << "\t" << ovr2.function3() << std::endl;
    std::cout << "============================OVERRIDE 3=============================================" << std::endl;
    std::cout << ovr3.function1(10) << "\t" << ovr3.function2(12, 5) << "\t" << ovr3.function3() << std::endl;
}