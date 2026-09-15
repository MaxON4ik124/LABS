#include <iostream>
#include <string>
#include <string.h>
using namespace std;


class MyString
{
    public:
        int capacity_;
        int size_;
        char* str;

    MyString()
    {
        this->str = new char[1];
        this->str[0] = '\0';
        this->capacity_ = 0;
        this->size_ = 0;
    }
    MyString(const char* source_str)
    {
        this->size_ = strlen(source_str);
        this->capacity_ = this->size_ + 1;
        this->str = new char[this->capacity_];
        strcpy(this->str, source_str);
    }
    MyString(const string& source_str)
    {
        const char* source_c_str = source_str.c_str();
        this->size_ = strlen(source_c_str);
        this->capacity_ = this->size_ + 1;
        this->str = new char[this->capacity_];
        strcpy(this->str, source_c_str);
    }
    MyString(const MyString& source_str)
    {
        this->size_ = source_str.size_;
        this->capacity_ = source_str.capacity_;
        this->str = new char[this->capacity_];
        memcpy(this->str, source_str.str, this->size_);
        this->str[this->size_] = '\0';
    }
    MyString(const char* source_str, int count)
    {
        this->str = new char[count+1];
        this->size_ = count;
        this->capacity_ = count+1;
        memcpy(this->str, source_str, count);
        this->str[count] = '\0';
    }
    MyString(const string& source_str, int count)
    {
        this->size_ = count;
        this->capacity_ = count + 1;
        this->str = new char[this->capacity_];
        memcpy(this->str, source_str.c_str(), count);
        this->str[count] = '\0';
    }
    MyString(const MyString& source_str, int count)
    {
        this->size_ = count;
        this->capacity_ = count + 1;
        this->str = new char[this->capacity_];
        memcpy(this->str, source_str.str, count);
        this->str[count] = '\0';
    }
    ~MyString()
    {
        delete[] this->str;
        this->str = nullptr;
        this->capacity_ = 0;
    }

    void clear()
    {
        delete[] this->str;
        this->str = new char[1];
        this->str[0] = '\0';
        this->size_ = 0;
    }

    void shrink_to_fit()
    {
        if (this->capacity_ > this->size_ + 1)
        {
            char* new_str = new char[this->size_ + 1];
            memcpy(new_str, this->str, this->size_);
            new_str[this->size_] = '\0';
            delete[] this->str;
            this->str = new_str;
            this->capacity_ = this->size_ + 1;
        }
    }

    MyString& operator=(const char* source_str)
    {
        int new_size_ = strlen(source_str);

        char* new_str = new char[new_size_ + 1];
        memcpy(new_str, source_str, new_size_);
        new_str[new_size_] = '\0';

        delete[] this->str;
        this->str = new_str;
        this->size_ = new_size_;
        this->capacity_ = new_size_ + 1;

        return *this;
    }

    MyString& operator=(const string& source_str)
    {
        const char* source_c_str = source_str.c_str();
        int new_size_ = strlen(source_c_str);
        char* new_str = new char[new_size_ + 1];
        memcpy(new_str, source_c_str, new_size_);
        new_str[new_size_] = '\0';

        delete[] this->str;
        this->str = new_str;
        this->size_ = new_size_;
        this->capacity_ = new_size_ + 1;

        return *this;
    }

    MyString& operator=(const MyString& source_str)
    {
        if (this != &source_str)
        {
            delete[] this->str;
            this->size_ = source_str.size_;
            this->capacity_ = source_str.capacity_;
            this->str = new char[this->capacity_];
            memcpy(this->str, source_str.str, this->size_);
            this->str[this->size_] = '\0';
        }
        return *this;
    }

    MyString& operator=(char ch)
    {
        delete[] this->str;
        this->str = new char[2];
        this->str[0] = ch;
        this->str[1] = '\0';
        this->size_ = 1;
        this->capacity_ = 2;
        return *this;
    }

    const char* c_str()
    {
        return this->str;
    }

    int size()
    {
        return this->size_;
    }

    int capacity()
    {
        return this->capacity_;
    }

    bool empty()
    {
        return this->size_ == 0;
    }

    void insert(int index, int cnt, char ch)
    {
        int new_size_ = this->size_ + cnt;
        char* new_str = new char[new_size_ + 1];
        memcpy(new_str, this->str, index);
        memset(new_str + index, ch, cnt);
        memcpy(new_str + index + cnt, this->str + index, this->size_ - index);
        new_str[new_size_] = '\0';
        delete[] this->str;
        this->str = new_str;
        this->size_ = new_size_;
        this->capacity_ = new_size_ + 1;
    }

    void insert(int index, const char* source_str)
    {
        int new_size = this->size_ + strlen(source_str);
        char* new_str = new char[new_size + 1];
        memcpy(new_str, this->str, index);
        memcpy(new_str + index, source_str, strlen(source_str));
        memcpy(new_str + index + strlen(source_str), this->str + index, this->size_ - index);
        new_str[new_size] = '\0';
        delete[] this->str;
        this->str = new_str;
        this->size_ = new_size;
        this->capacity_ = new_size + 1;
    }

    void insert(int index, string source_str)
    {
        insert(index, source_str.c_str());
    }

    void insert(int index, MyString source_str)
    {
        insert(index, source_str.c_str());
    }

    void insert(int index, const char* source_str, int count)
    {
        int new_size = this->size_ + count;
        char* new_str = new char[new_size + 1];
        memcpy(new_str, this->str, index);
        memcpy(new_str + index, source_str, count);
        memcpy(new_str + index + count, this->str + index, this->size_ - index);
        new_str[new_size] = '\0';
        delete[] this->str;
        this->str = new_str;
        this->size_ = new_size;
        this->capacity_ = new_size + 1;
    }

    void insert(int index, string source_str, int count)
    {
        insert(index, source_str.c_str(), count);
    }

    void insert(int index, MyString source_str, int count)
    {
        insert(index, source_str.c_str(), count);
    }

    void insert(int index, const char* source_str, int s_index, int count)
    {
        int new_size = this->size_ + count;
        char* new_str = new char[new_size + 1];
        char* temp = new char[count + 1];
        memcpy(new_str, source_str, index);
        memcpy(temp, source_str + s_index, count);
        memcpy(new_str + index, temp, count);
        memcpy(new_str + index + count, this->str + index, this->size_ - index);
        new_str[new_size] = '\0';
        delete[] this->str;
        this->str = new_str;
        this->size_ = new_size;
        this->capacity_ = new_size + 1;
    }

    void insert(int index, string source_str, int s_index, int count)
    {
        insert(index, source_str.c_str(), s_index, count);
    }

    void insert(int index, MyString source_str, int s_index, int count)
    {
        insert(index, source_str.c_str(), s_index, count);
    }

    void append(int count, char ch)
    {
        int new_size = this->size_ + count;
        char* new_str = new char[new_size + 1];
        memcpy(new_str, this->str, this->size_);
        memset(new_str + this->size_, ch, count);
        new_str[new_size] = '\0';
        delete[] this->str;
        this->str = new_str;
        this->size_ = new_size;
        this->capacity_ = new_size + 1;
    }

    void append(const char* source_str)
    {
        int new_size = this->size_ + strlen(source_str);
        strcat(this->str, source_str);
        this->str[new_size] = '\0';
        this->size_ = new_size;
        this->capacity_ = new_size + 1;
    }

    void append(string source_str)
    {
        append(source_str.c_str());
    }

    void append(MyString source_str)
    {
        append(source_str.c_str());
    }

    void append(const char* source_str, int count)
    {
        int new_size = this->size_ + count;
        strncat(this->str, source_str, count);
        this->str[new_size] = '\0';
        this->size_ = new_size;
        this->capacity_ = new_size + 1;
    }

    void append(string source_str, int count)
    {
        append(source_str.c_str(), count);
    }

    void append(MyString source_str, int count)
    {
        append(source_str.c_str(), count);
    }

    void append(const char* source_str, int s_index, int count)
    {
        int new_size = this->size_ + count;
        char* temp = new char[count];
        memcpy(temp, source_str + s_index, count);
        strncat(this->str, temp, count);
        this->str[new_size] = '\0';
        this->size_ = new_size;
        this->capacity_ = new_size + 1;
    }

    void erase(int index, int count)
    {
        int new_size = this->size_ - count;
        char* new_str = new char[capacity_];
        memcpy(new_str, this->str, index);
        memcpy(new_str + index, this->str + index + count, new_size - index);
        this->size_ = new_size;
        memset(new_str + new_size, 0, this->capacity_ - new_size);
        this->str = new_str;
    }

    void replace(int index, int count, const char* source_str)
    {
        int new_size = this->size_ - count + strlen(source_str);
        char* new_str = new char[new_size + 1];
        memcpy(new_str, this->str, index);
        strcat(new_str, source_str);
        strcat(new_str, this->str + index + count);
        this->str = new_str;
        this->size_ = new_size;
        this->capacity_ = new_size + 1;
    }

    void replace(int index, int count, string source_str)
    {
        replace(index, count, source_str.c_str());
    }

    void replace(int index, int count, MyString source_str)
    {
        replace(index, count, source_str.c_str());
    }

    void replace(int index, int count, const char* source_str, int s_count)
    {
        int new_size = this->size_ - count + s_count;
        if (new_size + 1 > this->capacity_) this->capacity_ = new_size + 1;
        char* new_str = new char[new_size + 1];
        memcpy(new_str, this->str, index);
        strncat(new_str, source_str, s_count);
        strcat(new_str, this->str + index + count);
        this->size_ = new_size;
        this->str = new_str; 
    }

    void replace(int index, int count, string source_str, int s_count)
    {
        replace(index, count, source_str.c_str(), s_count);
    }

    void replace(int index, int count, MyString source_str, int s_count)
    {
        replace(index, count, source_str.c_str(), s_count);
    }

    void replace(int index, int count, const char* source_str, int s_index, int s_count)
    {
        
    }

    
};

ostream& operator<<(ostream& out, const MyString& my_str)
{
    out << my_str.str;
    return out;
}

void pstr(const MyString& my_str)
{
    cout << "\"" << my_str.str << "\"" << " (" << my_str.size_ << ", " << my_str.capacity_ << ")" << endl;
}

int main()
{
    MyString str("hello amazing world");
    const char* s1 = "wonderful";
    str.replace(6, 7, s1, 6);
    pstr(str);


}
