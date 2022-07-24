// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../money_manager/money_manager.h"
#include <iostream>

int main()
{
    money_manager mgr;

    try
    {
        mgr.start();

        mgr.serve();
    }
    catch (const std::runtime_error& err)
    {
        std::cout << err.what() << " occurs during program execution, so it has to terminate now.\n";
        return -1;
    }
    
    return 0;
}