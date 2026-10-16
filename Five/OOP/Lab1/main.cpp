#include "MyString.h"
#include <iostream>

#if defined(_MSC_VER)
    #define NOINLINE_ATTR __declspec(noinline)
#else
    #define NOINLINE_ATTR __attribute__((noinline))
#endif

using namespace std;

NOINLINE_ATTR void modify(MyString& str) 
{
    if(str.size() == 0)
        str = "A picture is worth a thousand words.";
}

int main()
{
    MyString str;
    modify(str);
    cout << "String: " << str.c_str() << "; Size: " << str.size() << endl;
    
}