#include "MyString.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

string GetAllStdin()
{
    stringstream ss;
    ss << cin.rdbuf();
    return ss.str();
}



int main()
{
    string input = GetAllStdin();
    if(input.size() < 6)
        return 0;
    MyString str1;
    MyString str2("Test data");
    int op = input[0];
    int p_idx1 = input[1];
    int p_cnt1 = input[2];
    int p_idx2 = input[3];
    int p_cnt2 = input[4];
    MyString s_data(input.c_str() + 5, (int)input.size() - 5);


    try
    {
        str1 = s_data;
        str1.append("fazz");

        switch (op % 15)
        {
            case 0: str1.append(s_data); break;
            case 1: str1.append(s_data, p_idx1, p_cnt1); break;
            case 2: str1.insert(p_idx1 % (str1.size() + 1), s_data); break;
            case 3: str1.insert(p_idx1 % (str1.size() + 1), s_data, 'F'); break;
            case 4: str1.erase(p_idx1 % (str1.size() + 1), p_cnt1); break;
            case 5: str1.replace(p_idx1 % (str1.size() + 1), p_cnt1, s_data); break;
            case 6: str1.replace(p_idx1 % (str1.size() + 1), p_cnt1, s_data, p_idx2, p_cnt2); break;
            case 7:
            {
                MyString str3 = str1.substr(p_idx1 % (str1.size() + 1), p_cnt1); break;
            }
            case 8: str1.find(s_data); break;
            case 9: str1.find(s_data, p_idx1 % (str1.size() + 1)); break;
            case 10: str1.compare(str2); break;
            case 11: str1.at(p_idx1 % (str1.size() + 1)); break;
            case 12: str1.to_int(); break;
            case 13: str1.to_double(); break;
            case 14: str1.find_ac(s_data); break;
        }
    }
    catch (const out_of_range& e)
    {
    }
    catch (const MyString::MyStringConversionException& e)
    {
    }
    catch(exception& e)
    {
        return -1;
    }
    return 0;
}