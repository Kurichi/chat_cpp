#include <iostream>
#include <string>
#include "string.h"

int main()
{
    std::string str;
    std::cin >> str;
    std::cout << str.c_str() << std::endl;
}