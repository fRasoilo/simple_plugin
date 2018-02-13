//Example of a fourth plugin
#include <stdio.h>
#include "simple_plugin.h"
#include "fourth_plugin.h"

extern "C"
{
    fourth_plugin_api api;
    
     __declspec(dllexport) void my_print()
    {
        printf("I am the print from the fourth! \n");
    }

    //NECESSARY FUNCTION
    __declspec(dllexport) void load_fourth_plugin(APIRegistry *reg)
    {
        printf("I am the fourth one being loaded.\n");
        
        //Initing the api -- need to do this as well... ahhhhhh
        api.my_print = my_print;

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        //REGISTER_API(api_name)
        reg->add(FOURTH_PLUGIN_API_NAME,&api);
        //SP_REGISTER_API(reg, plugin_example_api);
    }

    //NECESSARY FUNCTION
    __declspec(dllexport) void unload_fourth_plugin(APIRegistry *reg)
    {
        printf("I am the fourth being cleaned!\n");
        
        //Any clean up or any other stuff you want to do here...

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        
        reg->remove(FOURTH_PLUGIN_API_NAME);
    }
   


}