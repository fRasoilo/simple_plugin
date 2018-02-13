//Example of a plugin

#include <stdio.h>
#include "simple_plugin.h"
#include "plugin_example.h"

extern "C"
{
    
    plugin_example_api api;

     __declspec(dllexport) void my_print()
    {
        printf("!!!!!!!!!! \n");
    }

    //NECESSARY FUNCTION
    __declspec(dllexport) void load_plugin_example(APIRegistry *reg)
    {
        printf("I have been loaded now I can do something!\n");
        
        //Initing the api -- need to do this as well... ahhhhhh
        api.my_print = my_print;

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        //REGISTER_API(api_name)
        reg->add(PLUGIN_EXAMPLE_API_NAME,&api);
        //SP_REGISTER_API(reg, plugin_example_api);
    }

    //NECESSARY FUNCTION
    __declspec(dllexport) void unload_plugin_example(APIRegistry *reg)
    {
        printf("I'll clean my stuff before being unloaded!\n");
        
        //Any clean up or any other stuff you want to do here...

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        
        reg->remove(PLUGIN_EXAMPLE_API_NAME);
    }
   


}