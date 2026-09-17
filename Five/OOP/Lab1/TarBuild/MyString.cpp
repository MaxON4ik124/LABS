#include "MyString.h"

MyString::MyString()
{
    this->str = new char[1];
    this->str[0] = '\0';
    this->capacity_ = 0;
    this->size_ = 0;
}
MyString::MyString(const char* source_str)
{
    this->size_ = strlen(source_str);
    this->capacity_ = this->size_ + 1;
    this->str = new char[this->capacity_];
    strcpy(this->str, source_str);
}
MyString::MyString(const string& source_str)
{
    const char* source_c_str = source_str.c_str();
    this->size_ = strlen(source_c_str);
    this->capacity_ = this->size_ + 1;
    this->str = new char[this->capacity_];
    strcpy(this->str, source_c_str);
}
MyString::MyString(const MyString& source_str)
{
    this->size_ = source_str.size_;
    this->capacity_ = source_str.capacity_;
    this->str = new char[this->capacity_];
    memcpy(this->str, source_str.str, this->size_);
    this->str[this->size_] = '\0';
}
MyString::MyString(const char* source_str, int count)
{
    this->str = new char[count+1];
    this->size_ = count;
    this->capacity_ = count+1;
    memcpy(this->str, source_str, count);
    this->str[count] = '\0';
}
MyString::MyString(const string& source_str, int count)
{
    this->size_ = count;
    this->capacity_ = count + 1;
    this->str = new char[this->capacity_];
    memcpy(this->str, source_str.c_str(), count);
    this->str[count] = '\0';
}
MyString::MyString(const MyString& source_str, int count)
{
    this->size_ = count;
    this->capacity_ = count + 1;
    this->str = new char[this->capacity_];
    memcpy(this->str, source_str.str, count);
    this->str[count] = '\0';
}
MyString::MyString(int count, char ch)
{
    if(count < 0)
        throw out_of_range("How can I create a string with negative amount of chararcters???");
    
    char* new_str = new char[count + 1];
    memset(new_str, ch, count);
    new_str[count] = '\0';
    this->str = new_str;
    this->size_ = count;
    this->capacity_ = count + 1;
    
}
MyString::~MyString()
{
    delete[] this->str;
    // this->str = nullptr;
    this->capacity_ = 0;
}

void MyString::clear()
{
    delete[] this->str;
    this->str = new char[1];
    this->str[0] = '\0';
    this->size_ = 0;
}

void MyString::shrink_to_fit()
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

void MyString::set_str(const char* source_str)
{
    if(source_str == nullptr)
        throw invalid_argument("String pointer cannot be null");

    int new_size = strlen(source_str);
    char* new_str = new char[new_size + 1];
    memcpy(new_str, source_str, new_size + 1);
    delete[] this->str;
    this->str = new_str;
    this->size_ = new_size;
    this->capacity_ = new_size + 1;
}

void MyString::set_size(int value)
{
    if(value < 0 || value >= capacity())
        throw out_of_range("Size is out of range");

    this->size_ = value;
    this->str[value] = '\0';
}

void MyString::set_capacity(int value)
{
    if(value < size() + 1)
        throw out_of_range("Capacity is too small for the string");

    if(value == capacity())
        return;

    char* new_str = new char[value];
    memcpy(new_str, c_str(), size() + 1);
    delete[] this->str;
    this->str = new_str;
    set_capacity(value);
}

const char* MyString::c_str() const
{
    return this->str;
}

int MyString::size() const
{
    return this->size_;
}

int MyString::capacity() const
{
    return this->capacity_;
}

bool MyString::empty() const
{
    return this->size_ == 0;
}

MyString& MyString::operator=(const char* source_str)
{
    int new_size = strlen(source_str);
    int new_capacity = new_size + 1 > this->capacity_ ? new_size + 1 : this->capacity_;
    char* new_str = new char[new_capacity];
    memcpy(new_str, source_str, new_size);
    new_str[new_size] = '\0';
    delete[] this->str;
    this->str = new_str;
    this->size_ = new_size;
    this->capacity_ = new_capacity;

    return *this;
}

MyString& MyString::operator=(const string& source_str)
{
    // const char* source_c_str = source_str.c_str();
    // int new_size_ = strlen(source_c_str);
    // char* new_str = new char[new_size_ + 1];
    // memcpy(new_str, source_c_str, new_size_);
    // new_str[new_size_] = '\0';

    // delete[] this->str;
    // this->str = new_str;
    // this->size_ = new_size_;
    // this->capacity_ = new_size_ + 1;
    
    operator=(source_str.c_str());
    return *this;
}

MyString& MyString::operator=(const MyString& source_str)
{
    // if (this != &source_str)
    // {
    //     delete[] this->str;
    //     this->size_ = source_str.size_;

    //     this->capacity_ = source_str.capacity_;
    //     this->str = new char[this->capacity_];
    //     memcpy(this->str, source_str.str, this->size_);
    //     this->str[this->size_] = '\0';
    // }

    operator=(source_str.str);
    return *this;
}

MyString& MyString::operator=(char ch)
{
    delete[] this->str;
    this->str = new char[2];
    this->str[0] = ch;
    this->str[1] = '\0';
    this->size_ = 1;
    this->capacity_ = 2;
    return *this;
}



void MyString::insert(int index, int cnt, char ch)
{
    if(index < 0 || index > this->size_ || cnt < 0)
        throw out_of_range("Maybe you re blind, BUT index is out of range!");
    
    int new_size = this->size_ + cnt;
    int new_capacity = new_size + 1 > this->capacity_ ? new_size + 1 : this->capacity_;
    char* new_str = new char[new_capacity];
    memcpy(new_str, this->str, index);
    memset(new_str + index, ch, cnt);
    memcpy(new_str + index + cnt, this->str + index, this->size_ - index);
    new_str[new_size] = '\0';
    delete[] this->str;
    this->str = new_str;
    this->size_ = new_size;
    this->capacity_ = new_capacity;
}

void MyString::insert(int index, const char* source_str, int s_index, int count)
{
    if(index < 0 || s_index < 0 || index > this->size_ || s_index > (int)strlen(source_str))
        throw out_of_range("Maybe you re blind, BUT index is out of range!");
    if(count < 0 || count + s_index > (int)strlen(source_str))
        throw out_of_range("How do you imagine me insert THIS count of chars? Very interesting...");
    
    int new_size = this->size_ + count;
    int new_capacity = new_size + 1 > this->capacity_ ? new_size + 1 : this->capacity_;

    char* new_str = new char[new_capacity];
    
    char* temp = new char[count + 1];
    memcpy(new_str, this->str, index);
    memcpy(temp, source_str + s_index, count);
    memcpy(new_str + index, temp, count);
    memcpy(new_str + index + count, this->str + index, this->size_ - index);
    new_str[new_size] = '\0';
    delete[] this->str;
    this->str = new_str;
    this->size_ = new_size;
    this->capacity_ = new_capacity;
    delete[] temp;
}

void MyString::insert(int index, const char* source_str)
{
    
    insert(index, source_str, 0, strlen(source_str));
    // int new_size = this->size_ + strlen(source_str);
    // char* new_str = new char[new_size + 1];
    // memcpy(new_str, this->str, index);
    // memcpy(new_str + index, source_str, strlen(source_str));
    // memcpy(new_str + index + strlen(source_str), this->str + index, this->size_ - index);
    // new_str[new_size] = '\0';
    // delete[] this->str;
    // this->str = new_str;
    // this->size_ = new_size;
    // this->capacity_ = new_size + 1;
}

void MyString::insert(int index, string source_str)
{
    insert(index, source_str.c_str());
}

void MyString::insert(int index, MyString source_str)
{
    insert(index, source_str.c_str());
}

void MyString::insert(int index, const char* source_str, int count)
{
    
    insert(index, source_str, 0, count);
    // int new_size = this->size_ + count;
    // char* new_str = new char[new_size + 1];
    // memcpy(new_str, this->str, index);
    // memcpy(new_str + index, source_str, count);
    // memcpy(new_str + index + count, this->str + index, this->size_ - index);
    // new_str[new_size] = '\0';
    // delete[] this->str;
    // this->str = new_str;
    // this->size_ = new_size;
    // this->capacity_ = new_size + 1;
}

void MyString::insert(int index, string source_str, int count)
{
    insert(index, source_str.c_str(), count);
}

void MyString::insert(int index, MyString source_str, int count)
{
    insert(index, source_str.c_str(), count);
}

void MyString::insert(int index, string source_str, int s_index, int count)
{
    insert(index, source_str.c_str(), s_index, count);
}

void MyString::insert(int index, MyString source_str, int s_index, int count)
{
    insert(index, source_str.c_str(), s_index, count);
}

void MyString::append(int count, char ch)
{
    if(count < 0)
        throw out_of_range("How do you imagine me append negative count of chars? Very interesting...");
    
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

void MyString::append(const char* source_str, int s_index, int count)
{       
    insert(this->size_, source_str, s_index, count);

    // int new_size = this->size_ + count;
    // char* temp = new char[count];
    // memcpy(temp, source_str + s_index, count);
    // strncat(this->str, temp, count);
    // this->str[new_size] = '\0';
    // this->size_ = new_size;
    // this->capacity_ = new_size + 1;
}

void MyString::append(const char* source_str)
{
    append(source_str, 0, strlen(source_str));
    // int new_size = this->size_ + strlen(source_str);
    // strcat(this->str, source_str);
    // this->str[new_size] = '\0';
    // this->size_ = new_size;
    // this->capacity_ = new_size + 1;
}

void MyString::append(string source_str)
{
    append(source_str.c_str());
}

void MyString::append(MyString source_str)
{
    append(source_str.c_str());
}

void MyString::append(const char* source_str, int count)
{    
    append(source_str, 0, count);
    // int new_size = this->size_ + count;
    // strncat(this->str, source_str, count);
    // this->str[new_size] = '\0';
    // this->size_ = new_size;
    // this->capacity_ = new_size + 1;
}

void MyString::append(string source_str, int count)
{
    append(source_str.c_str(), count);
}

void MyString::append(MyString source_str, int count)
{
    append(source_str.c_str(), count);
}

void MyString::erase(int index, int count)
{
    if(index + count > this->size_)
        throw out_of_range("How can I erase something after the end of string???");
    if(index < 0 || index >= this->size_)
        throw out_of_range("Maybe you re blind, BUT index is out of range!");
    
    int new_size = this->size_ - count;
    char* new_str = new char[capacity_];
    memcpy(new_str, this->str, index);
    memcpy(new_str + index, this->str + index + count, new_size - index);
    this->size_ = new_size;
    memset(new_str + new_size, 0, this->capacity_ - new_size);
    delete[] this->str;
    this->str = new_str;
}
void MyString::replace(int index, int count, const char* source_str, int s_index, int s_count)
{
    erase(index, count);
    insert(index, source_str, s_index, s_count);
    // int new_size = this->size_ - count + s_count;
    // if (new_size + 1 > this->capacity_) this->capacity_ = new_size + 1;
    // char* new_str = new char[new_size + 1];
    // memcpy(new_str, this->str, index);
    // memcpy(new_str + index, source_str + s_index, s_count);
    // strcat(new_str, this->str + index + count);
    // this->size_ = new_size;
    // this->str = new_str;
}

void MyString::replace(int index, int count, const char* source_str)
{
    replace(index, count, source_str, 0, (int)strlen(source_str));
    // int new_size = this->size_ - count + strlen(source_str);
    // char* new_str = new char[new_size + 1];
    // memcpy(new_str, this->str, index);
    // strcat(new_str, source_str);
    // strcat(new_str, this->str + index + count);
    // this->str = new_str;
    // this->size_ = new_size;
    // this->capacity_ = new_size + 1;
}

void MyString::replace(int index, int count, string source_str)
{
    replace(index, count, source_str.c_str());
}

void MyString::replace(int index, int count, MyString source_str)
{
    replace(index, count, source_str.c_str());
}

void MyString::replace(int index, int count, const char* source_str, int s_count)
{
    replace(index, count, source_str, 0, s_count);
    // int new_size = this->size_ - count + s_count;
    // if (new_size + 1 > this->capacity_) this->capacity_ = new_size + 1;
    // char* new_str = new char[new_size + 1];
    // memcpy(new_str, this->str, index);
    // strncat(new_str, source_str, s_count);
    // strcat(new_str, this->str + index + count);
    // this->size_ = new_size;
    // this->str = new_str; 
}

void MyString::replace(int index, int count, string source_str, int s_count)
{
    replace(index, count, source_str.c_str(), s_count);
}

void MyString::replace(int index, int count, MyString source_str, int s_count)
{
    replace(index, count, source_str.c_str(), s_count);
}

void MyString::replace(int index, int count, string source_str, int s_index, int s_count)
{
    replace(index, count, source_str.c_str(), s_index, s_count);
}
void MyString::replace(int index, int count, MyString source_str, int s_index, int s_count)
{
    replace(index, count, source_str.c_str(), s_index, s_count);
}

MyString MyString::substr(int index)
{
    MyString res;
    delete[] res.str;
    int substr_size = this->size_ - index;
    res.str = new char[substr_size + 1];
    memcpy(res.str, this->str + index, substr_size);
    res.str[substr_size] = '\0';
    res.size_ = substr_size;
    res.capacity_ = substr_size + 1;
    return res;
}

MyString MyString::substr(int index, int count)
{
    MyString res;
    delete[] res.str;
    res.str = new char[count + 1];
    memcpy(res.str, this->str + index, count);
    res.str[count] = '\0';
    res.size_ = count;
    res.capacity_ = count + 1;
    return res;
}

MyString MyString::operator+(const char* source_str)
{
    MyString res(*this);
    res += source_str;
    return res;
}

MyString MyString::operator+(string source_str)
{
    MyString res(*this);
    res += source_str;
    return res;
}

MyString MyString::operator+(MyString source_str)
{
    MyString res(*this);
    res += source_str;
    return res;
}

MyString& MyString::operator+=(const char* source_str)
{
    append(source_str);
    return *this;
}

MyString& MyString::operator+=(string source_str)
{
    append(source_str.c_str());
    return *this;
}

MyString& MyString::operator+=(MyString source_str)
{
    append(source_str.c_str());
    return *this;
}

char& MyString::operator[](int index)
{
    if(index < 0 || index >= this->size_)
        throw out_of_range("I can't get something from void...");
    
    return this->str[index];
}

int MyString::compare(MyString& comp_str)
{
    return strcmp(this->str, comp_str.c_str());
}

bool MyString::operator==(MyString& other) 
{
    return compare(other) == 0;
}

bool MyString::operator!=(MyString& other) 
{
    return compare(other) != 0;
}

bool MyString::operator<(MyString& other) 
{
    return compare(other) < 0;
}

bool MyString::operator>(MyString& other) 
{
    return compare(other) > 0;
}

bool MyString::operator<=(MyString& other) 
{
    return compare(other) <= 0;
}

bool MyString::operator>=(MyString& other) 
{
    return compare(other) >= 0;
}

int MyString::find(const char* source_str, int index)
{
    int src_len = strlen(source_str);
    int compared = 0;
    for(int i = index; i < this->size_; i++)
    {
        if(this->str[i] == source_str[compared])
        {
            int flag = 1;
            for(int j = 0;j < src_len;j++)
            {
                if(this->str[i] == source_str[j])
                {
                    compared++;
                    i++;
                }
                else
                {
                    i += (src_len - compared);
                    flag = 0;
                    break;
                }
            }
            compared = 0;
            if(flag == 1)
                return i - src_len;
        }
    }
    return 0;
}

int MyString::find(string source_str, int index)
{
    return find(source_str.c_str(), index);
}

int MyString::find(MyString source_str, int index)
{
    return find(source_str.c_str(), index);
}

int MyString::find(const char* source_str)
{
    return find(source_str, 0);
}

int MyString::find(string source_str)
{
    return find(source_str.c_str(), 0);
}

int MyString::find(MyString source_str)
{
    return find(source_str.c_str(), 0);
}

void pstr(const MyString& my_str)
{
    cout << "\"" << my_str.c_str() << "\"" << " (" << my_str.size() << ", " << my_str.capacity() << ")" << endl;
}

// int main()
// {
//     try
//     {
//         MyString str("abcde");
//         str.erase(2, 10);
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }
// }
