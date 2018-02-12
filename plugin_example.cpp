//Example of a plugin

#include <stdio.h>

extern "C"
{
    __declspec(dllexport) void my_print()
    {
        printf("$$$$$$ \n");
    }
}