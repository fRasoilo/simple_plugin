//------------------------------------------------------------------------------------------------------------
//This is an example of a plugin that can be used with the simple_plugin library.
//You can use this example a guide on how to write plugins or as a skeleton to be modified to your needs
//-------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------
//This is the c/cpp file for the plugin where the implementation lives:
//
// * Include the plugin header
//
//
//-------------------------------------------------------------------------------------------------------------

//Inlude the header for the plugin
#include "plugin_example.h"

//Include anything else you need for the plugin
#include <stdio.h>

    static plugin_state state; //plugin state (used to keep track of any plugin state and pass it around)

    void my_print()
    {
        printf("@@@@@@@  #######  @@@@@@@ \n");
    }

    void my_add_and_print(int a, int b)
    {
        printf("%d + %d  = %d \n", a,b, a + b);
    }

    //NECESSARY FUNCTION
    SP_EXPORT void load_plugin_example(APIRegistry *reg, bool32 reload = false)
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
    SP_EXPORT void unload_plugin_example(APIRegistry *reg, bool32 reload)
    {
        printf("I'll clean my stuff before being unloaded!\n");
        
        //Any clean up or any other stuff you want to do here...

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        
        reg->remove(PLUGIN_EXAMPLE_API_NAME, reload);
    }
   


}