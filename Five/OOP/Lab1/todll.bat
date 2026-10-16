g++ -std=c++17 -DMYSTRING_EXPORTS -c MyString.cpp -o MyString.o
g++ -shared -o MyString.dll MyString.o -Wl,--out-implib,libMyString.dll.a


