//Example of a plugin

#include <stdio.h>
#include "simple_plugin.h"
#include "plugin_example.h"

extern "C"
{
    
    //plugin_example_api plugin_example_api; // api instance
    static plugin_state state; //plugin state (used to keep track of any plugin state and pass it around)

     __declspec(dllexport) void my_print()
    {
        printf("() \n");
    }

    __declspec(dllexport) void my_add_and_print(int a, int b)
    {
        printf("%d + %d  = %d \n", a,b, a + b);
    }

    //NECESSARY FUNCTION
    __declspec(dllexport) void load_plugin_example(APIRegistry *reg, bool32 reload = false)
    {
        printf("I have been loaded now I can do something\n");
        
        //Initing the api
        SP_CREATE_API(plugin_example_api);

        //plugin_example_api.my_print = my_print;
        SP_INIT_API_FUNC_PTR(plugin_example_api,my_print);
        SP_INIT_API_FUNC_PTR(plugin_example_api,my_add_and_print);
        
        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        SP_REGISTER_API(reg, plugin_example_api, reload);
        //reg->add(PLUGIN_EXAMPLE_API_NAME, &api)
    }

    //NECESSARY FUNCTION
    __declspec(dllexport) void unload_plugin_example(APIRegistry *reg, bool32 reload)
    {
        printf("I'll clean my stuff before being unloaded!\n");
        
        //Any clean up or any other stuff you want to do here...

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        
        reg->remove(PLUGIN_EXAMPLE_API_NAME, reload);
    }
   


}