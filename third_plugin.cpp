//Example of a third plugin
#include <stdio.h>
#include "simple_plugin.h"
#include "third_plugin.h"

extern "C"
{
    third_plugin_api api;
    
     __declspec(dllexport) void my_print()
    {
        printf("I am the print from the third! \n");
    }

    //NECESSARY FUNCTION
    __declspec(dllexport) void load_third_plugin(APIRegistry *reg)
    {
        printf("I am the third one being loaded.\n");
        
        //Initing the api -- need to do this as well... ahhhhhh
        api.my_print = my_print;

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        //REGISTER_API(api_name)
        reg->add(THIRD_PLUGIN_API_NAME,&api);
        //SP_REGISTER_API(reg, plugin_example_api);
    }

    //NECESSARY FUNCTION
    __declspec(dllexport) void unload_third_plugin(APIRegistry *reg)
    {
        printf("I am the third being cleaned!\n");
        
        //Any clean up or any other stuff you want to do here...

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        
        reg->remove(THIRD_PLUGIN_API_NAME);
    }
   


}