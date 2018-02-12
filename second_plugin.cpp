//Example of a second plugin

#include <stdio.h>
#include "simple_plugin.h"

extern "C"
{
    //NECESSARY FUNCTION
    __declspec(dllexport) void load_second_plugin(APIRegistry *reg)
    {
        printf("I am the second plugin, I LIVE!\n");
        
        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        //REGISTER_API(api_name)
        reg->add("my_print");
    }

    __declspec(dllexport) void my_print()
    {
        printf("$$$$$$ \n");
    }
}