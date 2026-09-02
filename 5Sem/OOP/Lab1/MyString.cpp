#include <iostream>

using namespace std;

class MyString
{
    public:
        int capasity;
        string str;

    MyString(char* source_str)
    {
        this->str = source_str;
        this->capasity = str.length();
    }
    MyString(char* source_str, int capasity)
    {
        this->str = source_str;
        this->capasity = capasity;
    }    
}