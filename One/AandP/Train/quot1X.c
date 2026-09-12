#include <stdio.h>
// Hello world без строк и 4 printf'a максимум
// int main()
// {
//     unsigned int a = 'leH';
//     unsigned int b = ' ol';
//     unsigned int c = 'roW';
//     unsigned int d = '!dl';
//     printf((char*)&a);
//     printf((char*)&b);
//     printf((char*)&c);
//     printf((char*)&d);
// }
int main()
{
    char c1 = 'l';
    char c2 = 'e';
    char c3 = 'H';
    int s1 = (c1 << 16) | (c2 << 8) | c3;
    printf((char*)&s1);
    c1 = ' ';
    c2 = 'o';
    c3 = 'l';
    s1 = (c1 << 16) | (c2 << 8) | c3;
    printf((char*)&s1);
    c1 = 'r';
    c2 = 'o';
    c3 = 'w';
    s1 = (c1 << 16) | (c2 << 8) | c3;
    printf((char*)&s1);
    c1 = '!';
    c2 = 'd';
    c3 = 'l';
    s1 = (c1 << 16) | (c2 << 8) | c3;
    printf((char*)&s1);
}
// Сложность O(1)
// Инвариант: (char*)&s1 = c1 + c2 + c3