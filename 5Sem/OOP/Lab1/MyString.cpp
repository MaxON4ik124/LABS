#include <iostream>

using namespace std;

class MyString
{
    public:
        int capacity;
        string str;

    MyString(char* source_str)
    {
        this->str = source_str;
        this->capacity = str.length();
    }
    MyString(char* source_str, int capacity)
    {
        this->str = source_str;
        this->capacity = capacity;
    }    
};