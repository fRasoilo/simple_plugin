//Example of host loading a plugin to use it's API

#include<Window.h>

#include <stdio.h>


#include "sample_plugin.h"



#include "sample_plugin.h"

char* sample_plugin = "sample_plugin.dll";


int main()
{

    //Simplest API 
    //Let the library manage the plugin, we load the plugin we want and then request it's API from the library.
    //We don't need to keep track of the plugin.
    bool32 reloadable = true;
    sp_load_plugin(plugin_name, reloadable);
    sample_plugin_api *sample_api = (sample_plugin_api*)sp_get_api(SAMPLE_PLUGIN_API_NAME);
    sample_api->my_print();
    sample_api->my_add_and_print(2,3);

    //If we want we can also keep a pointer to the plugin. The library will still keep track of it internally, but we can access the plugin directly.
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
    APIRegistry my_registry = sp_registry_create(args); //capacity
    
    //Now you can do all the same operations as above passing in your created registry.
    sp_load_plugin(&my_registry,plugin_name, reloadable);
    sample_plugin_api *sample_api = (sample_plugin_api*)sp_get_api(&my_registry,SAMPLE_PLUGIN_API_NAME);
    sample_api->my_print();
    sample_api->my_add_and_print(2,3);


    plugin = sp_load_plugin(&my_registry,second_plugin, !reloadable);
    second_plugin_api *second_api = (second_plugin_api*)sp_get_api(&my_registry,plugin);
    second_api->my_second_print();
    
    sp_unload_plugin(&my_registry,plugin);
    //We can also unload it trough the API name
    sp_unload_plugin(&my_registry,SAMPLE_PLUGIN_API_NAME);


    //
    //Hot Reloading
    //
    
    //Let's load up the sample_plugin again to demonstrate hot-reloading
    sp_load_plugin(plugin_name, reloadable);
    sample_plugin_api *sample_api = (sample_plugin_api*)sp_get_api(SAMPLE_PLUGIN_API_NAME);
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