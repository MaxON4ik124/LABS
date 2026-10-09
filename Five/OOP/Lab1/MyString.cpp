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
    operator=(source_str.c_str());
    return *this;
}

MyString& MyString::operator=(const MyString& source_str)
{
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
}

void MyString::append(const char* source_str)
{
    append(source_str, 0, strlen(source_str));
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
}

void MyString::replace(int index, int count, const char* source_str)
{
    replace(index, count, source_str, 0, (int)strlen(source_str));
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

int MyString::find_ac(const char* source_str)
{
    if (source_str == nullptr)
        throw invalid_argument("I will not find nothing...");

    const int pattern_size = strlen(source_str);
    if (pattern_size == 0)
        return 0;

    struct Node
    {
        int next[256];
        int link;

        Node() : link(0)
        {
            for (int& transition : next)
                transition = -1;
        }
    };

    vector<Node> trie(pattern_size + 1);

    
    int state = 0;
    for (int i = 0; i < pattern_size; ++i)
    {
        const unsigned char symbol = static_cast<unsigned char>(source_str[i]);

        if (trie[state].next[symbol] == -1)
        {
            trie[state].next[symbol] = i + 1;
            trie[i + 1].link = 0;
        }

        state = trie[state].next[symbol];
    }


    queue<int> states;
    for (int symbol = 0; symbol < 256; ++symbol)
    {
        int& transition = trie[0].next[symbol];

        if (transition == -1)
            transition = 0;
        else
            states.push(transition);
    }

    while (!states.empty())
    {
        const int current = states.front();
        states.pop();

        for (int symbol = 0; symbol < 256; ++symbol)
        {
            int& transition = trie[current].next[symbol];

            if (transition == -1)
            {
                transition = trie[trie[current].link].next[symbol];
            }
            else
            {
                trie[transition].link =
                    trie[trie[current].link].next[symbol];
                states.push(transition);
            }
        }
    }
    state = 0;
    for (int i = 0; i < size_; ++i)
    {
        const unsigned char symbol = static_cast<unsigned char>(str[i]);
        state = trie[state].next[symbol];

        if (state == pattern_size)
            return i - pattern_size + 1;
    }
    return 0;
}

int MyString::find_ac(string source_str)
{
    return find_ac(source_str.c_str());
}

int MyString::find_ac(MyString source_str)
{
    return find_ac(source_str.c_str());
}

int MyString::find(string source_str)
{
    return find(source_str.c_str(), 0);
}

int MyString::find(MyString source_str)
{
    return find(source_str.c_str(), 0);
}

MyString::MyString(MyString&& source_str) noexcept : 
    capacity_(source_str.capacity_), 
    size_(source_str.size_), 
    str(source_str.str) 
    {
    source_str.capacity_ = 0;
    source_str.size_ = 0;
    source_str.str = nullptr;
}

MyString& MyString::operator=(MyString&& source_str) noexcept
{
    if (this != &source_str)
    {
        delete[] this->str;

        this->capacity_ = source_str.capacity_;
        this->size_ = source_str.size_;
        this->str = source_str.str;

        source_str.capacity_ = 0;
        source_str.size_ = 0;
        source_str.str = nullptr;
    }
    return *this;
}

MyString::MyString(int num) : MyString()
{
    char buf[32];
    sprintf(buf, "%d", num);
    set_str(buf);
}
MyString::MyString(double num) : MyString()
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%.12g", num);
    set_str(buf);
}
ostream& operator<<(ostream& out, const MyString& str)
{
    out << str.c_str();
    return out;
}

istream& operator>>(istream& in, MyString& str)
{
    std::string temp;
    in >> temp;
    str = temp;
    return in;
}

char& MyString::at(int index)
{
    if(index < 0 || index >= this->size_)
        throw out_of_range("There is nothing here    . . . . ");
    return this->str[index];
}

int MyString::to_int()
{
    if(this->size_ == 0)
        throw out_of_range("I can't convert nothing to int");
    try
    {
        string temp(this->str, this->size_);
        size_t val;
        return stoi(temp, &val);
    }
    catch (const invalid_argument&)
    {
        throw invalid_argument("This is not a digit...");
    }
    catch (const out_of_range&)
    {
        throw out_of_range("This digit is toooooo big for int");
    }
}

double MyString::to_double()
{
    if(this->size_ == 0)
        throw out_of_range("I can't convert nothing to int");
    try
    {
        string temp(this->str, this->size_);
        return stod(temp);
    }
    catch (const invalid_argument&)
    {
        throw invalid_argument("This is not a digit...");
    }
    catch (const out_of_range&)
    {
        throw out_of_range("This digit is toooooo big for int");
    }
}

MyString::iterator MyString::begin() 
{
    return str;
}

MyString::iterator MyString::end() 
{
    return str + this->size();
}

MyString::const_iterator MyString::cbegin()
{
    return str;
}

MyString::const_iterator MyString::cend()
{
    return str + this->size();
}

MyString::reverse_iterator MyString::rbegin()
{
    return reverse_iterator(end());
}

MyString::reverse_iterator MyString::rend()
{
    return reverse_iterator(begin());
}

MyString::const_reverse_iterator MyString::rcbegin()
{
    return const_reverse_iterator(end());
}

MyString::const_reverse_iterator MyString::rcend()
{
    return const_reverse_iterator(begin());
}

int MyString::iterator2index(iterator pos)
{
    return pos - begin();
}

int MyString::iterator2index(const_iterator pos)
{
    return pos - begin();
}

int MyString::iterator2index(reverse_iterator pos)
{
    return pos.base() - begin() - 1;
}

int MyString::iterator2index(const_reverse_iterator pos)
{
    return pos.base() - begin() - 1;
}

void MyString::insert(iterator pos, int count, char ch)
{
    insert(iterator2index(pos), count, ch);
}
void MyString::insert(iterator pos, const char* source_str, int source_index, int count)
{
    insert(iterator2index(pos), source_str, source_index, count);
}
void MyString::insert(iterator pos, const char* source_str)
{
    insert(iterator2index(pos), source_str);
}
void MyString::insert(iterator pos, std::string source_str)
{
    insert(iterator2index(pos), source_str);
}
void MyString::insert(iterator pos, MyString source_str)
{
    insert(iterator2index(pos), source_str);
}
void MyString::insert(iterator pos, const char* source_str, int count)
{
    insert(iterator2index(pos), source_str, count);
}
void MyString::insert(iterator pos, std::string source_str, int count)
{
    insert(iterator2index(pos), source_str, count);
}
void MyString::insert(iterator pos, MyString source_str, int count)
{
    insert(iterator2index(pos), source_str, count);
}
void MyString::insert(iterator pos, std::string source_str, int source_index, int count)
{
    insert(iterator2index(pos), source_str, source_index, count);
}
void MyString::insert(iterator pos, MyString source_str, int source_index, int count)
{
    insert(iterator2index(pos), source_str, source_index, count);
}

void MyString::insert(const_iterator pos, const char* source_str)
{
    insert(iterator2index(pos), source_str);
}

void MyString::insert(reverse_iterator pos, const char* source_str)
{
    insert(iterator2index(pos), source_str);
}

void MyString::insert(const_reverse_iterator pos, const char* source_str)
{
    insert(iterator2index(pos), source_str);
}

void MyString::erase(iterator pos, int count)
{
    erase(iterator2index(pos), count);
}
void MyString::replace(iterator pos, int count, const char* source_str,
                       int source_index, int source_count)
{
    replace(iterator2index(pos), count, source_str, source_index, source_count);
}

void MyString::replace(iterator pos, int count, const char* source_str)
{
    replace(iterator2index(pos), count, source_str);
}

void MyString::replace(iterator pos, int count, std::string source_str)
{
    replace(iterator2index(pos), count, source_str);
}

void MyString::replace(iterator pos, int count, MyString source_str)
{
    replace(iterator2index(pos), count, source_str);
}

void MyString::replace(iterator pos, int count, const char* source_str,
                       int source_count)
{
    replace(iterator2index(pos), count, source_str, source_count);
}

void MyString::replace(iterator pos, int count, std::string source_str,
                       int source_count)
{
    replace(iterator2index(pos), count, source_str, source_count);
}

void MyString::replace(iterator pos, int count, MyString source_str,
                       int source_count)
{
    replace(iterator2index(pos), count, source_str, source_count);
}

void MyString::replace(iterator pos, int count, std::string source_str,
                       int source_index, int source_count)
{
    replace(iterator2index(pos), count, source_str, source_index, source_count);
}

void MyString::replace(iterator pos, int count, MyString source_str,
                       int source_index, int source_count)
{
    replace(iterator2index(pos), count, source_str, source_index, source_count);
}


void pstr(const MyString& my_str)
{
    cout << "\"" << my_str.c_str() << "\"" << " (" << my_str.size() << ", " << my_str.capacity() << ")" << endl;
}


