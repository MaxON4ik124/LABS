#include "MyString.h"
#include <iostream>

int main()
{
    MyString test("ABcABcABcAAA");
    cout << test.rfind("c");
}