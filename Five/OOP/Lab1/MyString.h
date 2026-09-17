#ifndef MY_STRING_H
#define MY_STRING_H

#include <iostream>
#include <string>
#include <string.h>
#include <stdexcept>


using namespace std;


class MyString
{
private:
    int capacity_;
    int size_;
    char* str;

public:
    MyString();
    MyString(const char* source_str);
    MyString(const std::string& source_str);
    MyString(const MyString& source_str);
    MyString(const char* source_str, int count);
    MyString(const std::string& source_str, int count);
    MyString(const MyString& source_str, int count);
    MyString(int count, char ch);
    ~MyString();

    void clear();
    void shrink_to_fit();

    void set_str(const char* source_str);
    void set_size(int value);
    void set_capacity(int value);

    MyString& operator=(const char* source_str);
    MyString& operator=(const std::string& source_str);
    MyString& operator=(const MyString& source_str);
    MyString& operator=(char ch);

    const char* c_str() const;
    int size() const;
    int capacity() const;
    bool empty() const;

    void insert(int index, int count, char ch);
    void insert(int index, const char* source_str, int source_index, int count);
    void insert(int index, const char* source_str);
    void insert(int index, std::string source_str);
    void insert(int index, MyString source_str);
    void insert(int index, const char* source_str, int count);
    void insert(int index, std::string source_str, int count);
    void insert(int index, MyString source_str, int count);
    void insert(int index, std::string source_str, int source_index, int count);
    void insert(int index, MyString source_str, int source_index, int count);

    void append(int count, char ch);
    void append(const char* source_str, int source_index, int count);
    void append(const char* source_str);
    void append(std::string source_str);
    void append(MyString source_str);
    void append(const char* source_str, int count);
    void append(std::string source_str, int count);
    void append(MyString source_str, int count);

    void erase(int index, int count);

    void replace(int index, int count, const char* source_str, int source_index, int source_count);
    void replace(int index, int count, const char* source_str);
    void replace(int index, int count, std::string source_str);
    void replace(int index, int count, MyString source_str);
    void replace(int index, int count, const char* source_str, int source_count);
    void replace(int index, int count, std::string source_str, int source_count);
    void replace(int index, int count, MyString source_str, int source_count);
    void replace(int index, int count, std::string source_str, int source_index, int source_count);
    void replace(int index, int count, MyString source_str, int source_index, int source_count);

    MyString substr(int index);
    MyString substr(int index, int count);

    MyString operator+(const char* source_str);
    MyString operator+(std::string source_str);
    MyString operator+(MyString source_str);
    MyString& operator+=(const char* source_str);
    MyString& operator+=(std::string source_str);
    MyString& operator+=(MyString source_str);

    char& operator[](int index);

    int compare(MyString& comp_str);
    bool operator==(MyString& other);
    bool operator!=(MyString& other);
    bool operator<(MyString& other);
    bool operator>(MyString& other);
    bool operator<=(MyString& other);
    bool operator>=(MyString& other);

    int find(const char* source_str, int index);
    int find(std::string source_str, int index);
    int find(MyString source_str, int index);
    int find(const char* source_str);
    int find(std::string source_str);
    int find(MyString source_str);
};

void pstr(const MyString& my_str);

#endif
