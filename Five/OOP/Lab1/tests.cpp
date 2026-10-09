#include "MyString.h"

int passed = 0;
int total = 0;

void isPass(bool test)
{
    total++;
    if(test)
    {
        cout << '.';
        passed++;
    }
    else
    {
        cout << 'F';
    }
}

int main()
{

    MyString test1("Moving string!");
    MyString test1_1 = move(test1);

    isPass(test1.size() == 0 && test1_1.size() == 14);
    test1.clear();
    test1_1.clear();

    MyString test2(0b10101);
    MyString test2_1(64534);
    MyString test2_2(0x12345678);
    isPass(test2.size() == 2 && test2_1.size() == 5 && test2_2.size() == 9);
    test2.clear();
    test2_1.clear();
    test2_2.clear();

    MyString test3(12.0);
    MyString test3_1(123.12);
    MyString test3_2(5345.24321);
    isPass(test3.size() == 2 && test3_1.size() == 6 && test3_2.size() == 10);
    test3.clear();
    test3_1.clear();
    test3_2.clear();

    ifstream input("input.txt");
    ofstream output("output.txt");
    MyString test4;
    input >> test4;
    isPass(test4.size() == 21);
    output << test4;
    output.close();
    ifstream outcheck("output.txt");
    string check;
    outcheck >> check;
    isPass(check.size() == 21);
    input.close();
    outcheck.close();

    MyString test5("Very very very big or massive string");
    MyString test5_1("big");
    int ind = test5.find_ac(test5_1);
    isPass(ind == 15);
    test5.clear();
    test5_1.clear();

    MyString test6("abcdefg");
    isPass(test6.at(4) == 'e');
    test6.clear();

    MyString test7("12345");
    MyString test8("133.7");
    isPass(test7.to_int() == 12345);
    isPass(test8.to_double() == 133.7);

    MyString insert_fwd_1("abcd");
    MyString::iterator fwd_1 = insert_fwd_1.begin() + 2;
    isPass((insert_fwd_1.insert(fwd_1, "X"),
            strcmp(insert_fwd_1.c_str(), "abXcd") == 0));

    MyString insert_fwd_2("abcd");
    MyString::iterator fwd_2 = insert_fwd_2.begin();
    isPass((insert_fwd_2.insert(fwd_2, "START"),
            strcmp(insert_fwd_2.c_str(), "STARTabcd") == 0));

    MyString insert_fwd_3("abcd");
    MyString::const_iterator fwd_3 = insert_fwd_3.cbegin() + 3;
    isPass((insert_fwd_3.insert(fwd_3, "Y"),
            strcmp(insert_fwd_3.c_str(), "abcYd") == 0));

    MyString insert_fwd_4("abcd");
    MyString::const_iterator fwd_4 = insert_fwd_4.cend();
    isPass((insert_fwd_4.insert(fwd_4, "END"),
            strcmp(insert_fwd_4.c_str(), "abcdEND") == 0));

    MyString insert_bwd_1("abcd");
    MyString::reverse_iterator bwd_1 = insert_bwd_1.rbegin();
    insert_bwd_1.insert(bwd_1, "X");
    isPass(strcmp(insert_bwd_1.c_str(), "abcXd") == 0);

    MyString insert_bwd_2("abcd");
    MyString::reverse_iterator bwd_2 = insert_bwd_2.rbegin() + 1;
    isPass((insert_bwd_2.insert(bwd_2, "Y"),
            strcmp(insert_bwd_2.c_str(), "abYcd") == 0));

    MyString insert_bwd_3("abcd");
    MyString::const_reverse_iterator bwd_3 = insert_bwd_3.rcbegin() + 2;
    isPass((insert_bwd_3.insert(bwd_3, "Z"),
            strcmp(insert_bwd_3.c_str(), "aZbcd") == 0));

    MyString insert_bwd_4("abcd");
    MyString::const_reverse_iterator bwd_4 = insert_bwd_4.rcbegin();
    isPass((insert_bwd_4.insert(bwd_4, "END"),
            strcmp(insert_bwd_4.c_str(), "abcENDd") == 0));


    cout << endl << "======================================================================" << endl;
    cout << "Passed: " << passed << "/" << total << endl;


}
