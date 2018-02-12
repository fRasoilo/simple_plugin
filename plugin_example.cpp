//Example of a plugin

#include <stdio.h>
#include "simple_plugin.h"

extern "C"
{
    //NECESSARY FUNCTION
    __declspec(dllexport) void load_plugin_example(APIRegistry *reg)
    {
        printf("I have been loaded now I can do something!\n");
        
        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        //REGISTER_API(api_name)
        reg->add("my_print");
    }

    __declspec(dllexport) void my_print()
    {
        printf("$$$$$$ \n");
    }
}