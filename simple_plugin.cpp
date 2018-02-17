//Example of host loading a plugin to use it's API

#include<Windows.h>

#include <stdio.h>

#define SIMPLE_PLUGIN_IMPLEMENTATION
#include "simple_plugin.h"



#include "sample_plugin.h"
#include "second_plugin.h"

char* sample_plugin = "sample_plugin.dll";
char* second_plugin = "second_plugin.dll";

int main()
{

    //Simplest API 
    //Let the library manage the plugin, we load the plugin we want and then request it's API from the library.
    //We don't need to keep track of the plugin.
    bool32 reloadable = true;
    sp_load_plugin(sample_plugin, reloadable);

    //We look at the plugin header to see the type of the api, the convention is that the 
    //api struct will be plugin_name_api, so in this case sample_plugin_api.
    //That way we can cast the void* that sp_get_api returns to us and store it in the appropriate
    //type.
    sample_plugin_api *sample_api = (sample_plugin_api*)sp_get_api(SAMPLE_PLUGIN_API_NAME);
    sample_api->my_print();
    sample_api->my_add_and_print(2,3);

    //If we want we can also keep a pointer to the plugin. The library will still keep track of it internally, but we can access the plugin directly.
    //Again, in this case we went to the second_plugin header to see what kind of type the api struct it provides is.
    //the convention is plugin_name_api so in this case we get second_plugin_api.
    SPlugin *plugin = sp_load_plugin(second_plugin, !reloadable);
    second_plugin_api *second_api = (second_plugin_api*)sp_get_api(plugin);
    second_api->my_second_print();
    
    //If we know we are done with a plugin, we can unload it manually. Otherwise leave it for the library to clean up at shutdown time.
    sp_unload_plugin(plugin);
    //We can also unload it trough the API name
    sp_unload_plugin(SAMPLE_PLUGIN_API_NAME);

    //
    //Using our registry
    //

    //The examples above use the default registry provided by the library.
    //You can also create instances of an APIRegistry and use them instead of the default library one.
    //For example if you would like to create a separete registry for different kinds of plugins.
    APIRegistry my_registry = sp_registry_create(10); //capacity
    
    //Now you can do all the same operations as above passing in your created registry.
    sp_load_plugin(&my_registry,sample_plugin, reloadable);
    sample_api = (sample_plugin_api*)sp_get_api(&my_registry,SAMPLE_PLUGIN_API_NAME);
    sample_api->my_print();
    sample_api->my_add_and_print(2,3);


    plugin = sp_load_plugin(&my_registry,second_plugin, !reloadable);
    second_api = (second_plugin_api*)sp_get_api(&my_registry,plugin);
    second_api->my_second_print();
    
    sp_unload_plugin(&my_registry,plugin);
    //We can also unload it trough the API name
    sp_unload_plugin(&my_registry,SAMPLE_PLUGIN_API_NAME);

    sp_registry_destroy(&my_registry);


    //
    //Hot Reloading
    //
    
    //Let's load up the sample_plugin again to demonstrate hot-reloading
    sp_load_plugin(sample_plugin, reloadable);
    sample_api = (sample_plugin_api*)sp_get_api(SAMPLE_PLUGIN_API_NAME);
    sample_api->my_print();
    
    while(1)
    {
        Sleep(100);
        //Run the registry update, it will check all reloadable plugins for changes.
        sp_update(); 
        //We need to get the api again in case the plugin has been modified
        sample_plugin_api *sample_api = (sample_plugin_api*)sp_get_api(SAMPLE_PLUGIN_API_NAME);
        sample_api->my_print();
        

        //More code here...
        
    }

    return 0;
}