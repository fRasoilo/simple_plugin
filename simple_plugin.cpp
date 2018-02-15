//Example of host loading a plugin to use it's API

#include<Window.h>

#include <stdio.h>

#include "sample_plugin.h"


#include "simple_plugin.h"

char* sample_plugin = "sample_plugin.dll";
char* second_plugin = "second_plugin.dll";
char* third_plugin = "third_plugin.dll";
char* fourth_plugin = "fourth_plugin.dll";


int main()
{

    //Sample Plugin
    bool32 result = sp_win32_load_plugin(plugin_name, true);
    plugin_example_api *pe = (plugin_example_api*)sp_registry.get(PLUGIN_EXAMPLE_API_NAME);
    pe->my_print();
    pe->my_add_and_print(2,3);
   
    //Loop forever until we break out. This is used to demonstrate the hot-reloading option.
    while(1)
    {
        Sleep(100);
        sp_internal_api_registry_check_reloadable_plugins();
        pe = (plugin_example_api*)sp_registry.get(PLUGIN_EXAMPLE_API_NAME);
        pe->my_print();

    }

    return 0;
}