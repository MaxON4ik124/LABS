#include <iostream>
#include <string>
#include <string.h>
using namespace std;


class MyString
{
    public:
        int capacity;
        int size;
        char* str;

    MyString()
    {
        this->str = new char[1];
        this->str[0] = '\0';
        this->capacity = 0;
        this->size = 0;
    }
    MyString(const char* source_str)
    {
        this->size = strlen(source_str);
        this->capacity = this->size + 1;
        this->str = new char[this->capacity];
        strcpy(this->str, source_str);
    }
    MyString(const string& source_str)
    {
        this->size = static_cast<int>(source_str.size());
        this->capacity = this->size + 1;
        this->str = new char[this->capacity];

        for (int i = 0; i < this->size; i++) this->str[i] = source_str[i];
        this->str[this->size] = '\0';
    }
    MyString(const MyString& source_str)
    {
        this->size = source_str.size;
        this->capacity = source_str.capacity;
        this->str = new char[this->capacity];
        for (int i = 0; i < this->size; i++) this->str[i] = source_str.str[i];
        this->str[this->size] = '\0';
    }
    MyString(const char* source_str, int count)
    {
        this->str = new char[count+1];
        this->size = count;
        this->capacity = count+1;
        for (int i = 0; i < count; i++) this->str[i] = source_str[i];
        this->str[count] = '\0';
    }
    MyString(const string& source_str, int count)
    {
        this->size = count;
        this->capacity = count + 1;
        this->str = new char[this->capacity];
        for (int i = 0; i < count; i++) this->str[i] = source_str[i];
        this->str[count] = '\0';
    }
    MyString(const MyString& source_str, int count)
    {
        this->size = count;
        this->capacity = count + 1;
        this->str = new char[this->capacity];
        for (int i = 0; i < count; i++) this->str[i] = source_str.str[i];
        this->str[count] = '\0';
    }
    ~MyString()
    {
        delete[] this->str;
        this->str = nullptr;
        this->capacity = 0;
    }

    void clear()
    {
        delete[] this->str;
        this->str = new char[1];
        this->str[0] = '\0';
        this->size = 0;
    }

    void shrink_to_fit()
    {
        if (this->capacity > this->size + 1)
        {
            char* new_str = new char[this->size + 1];
            for (int i = 0; i < this->size; i++) new_str[i] = this->str[i];
            new_str[this->size] = '\0';
            delete[] this->str;
            this->str = new_str;
            this->capacity = this->size + 1;
        }
    }

    MyString& operator=(const char* source_str)
    {
        int new_size = 0;
        while (source_str[new_size] != '\0') new_size++;

        char* new_str = new char[new_size + 1];
        for (int i = 0; i < new_size; i++) new_str[i] = source_str[i];
        new_str[new_size] = '\0';

        delete[] this->str;
        this->str = new_str;
        this->size = new_size;
        this->capacity = new_size + 1;

        return *this;
    }

    MyString& operator=(const string& source_str)
    {
        int new_size = 0;
        while (source_str[new_size] != '\0') new_size++;
        char* new_str = new char[new_size + 1];
        for (int i = 0; i < new_size; i++) new_str[i] = source_str[i];
        new_str[new_size] = '\0';

        delete[] this->str;
        this->str = new_str;
        this->size = new_size;
        this->capacity = new_size + 1;

        return *this;
    }

    MyString& operator=(const MyString& source_str)
    {
        if (this != &source_str)
        {
            delete[] this->str;
            this->size = source_str.size;
            this->capacity = source_str.capacity;
            this->str = new char[this->capacity];
            for (int i = 0; i < this->size; i++) this->str[i] = source_str.str[i];
            this->str[this->size] = '\0';
        }
        return *this;
    }

    MyString& operator=(char ch)
    {
        delete[] this->str;
        this->str = new char[2];
        this->str[0] = ch;
        this->str[1] = '\0';
        this->size = 1;
        this->capacity = 2;
        return *this;
    }

    const char* c_str()
    {
        return this->str;
    }

    int length()
    {
        return this->size;
    }

    int volume()
    {
        return this->capacity;
    }

    bool empty()
    {
        return this->size == 0;
    }

    void insert(int index, int cnt, char ch)
    {
        int new_size = this->size + cnt;
        char* new_str = new char[new_size + 1];
        strncpy(new_str, this->str, index);
        for (int i = 0; i < cnt; i++) new_str[index + i] = ch;
        strncpy(new_str + index + cnt, this->str + index, this->size - index);
        new_str[new_size] = '\0';
        delete[] this->str;
        this->str = new_str;
        this->size = new_size;
        this->capacity = new_size + 1;
    }

    
};

ostream& operator<<(ostream& out, const MyString& my_str)
{
    out << my_str.str;
    return out;
}

void pstr(const MyString& my_str)
{
    cout << "\"" << my_str.str << "\"" << " (" << my_str.size << ", " << my_str.capacity << ")" << endl;
}

int main()
{
    MyString str("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    printf("%d", sizeof(str));
    // str = "aaaaa";
    // cout << str.c_str() << endl;
    // str.insert(0,1,'!');
    // pstr(str);
    // str.insert(3,2,'@');
    // pstr(str);

}
