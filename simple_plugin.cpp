//Example of host loading a plugin

#include <Windows.h>

#include <stdio.h>

#include "plugin_example.h"
#include "second_plugin.h"
#include "third_plugin.h"
#include "fourth_plugin.h"

#include "simple_plugin.h"

char* plugin_name = "plugin_example.dll";
char* second_plugin = "second_plugin.dll";
char* third_plugin = "third_plugin.dll";
char* fourth_plugin = "fourth_plugin.dll";


int main()
{

    //First Plugin
    bool32 result = sp_win32_load_plugin(plugin_name, true);
    plugin_example_api *pe = (plugin_example_api*)sp_registry.get(PLUGIN_EXAMPLE_API_NAME);
    pe->my_print();
    
    //Second Plugin
    result = sp_win32_load_plugin(second_plugin, true);

   

    //Use second plugin
    second_plugin_api *sp = (second_plugin_api *)sp_registry.get(SECOND_PLUGIN_API_NAME);
    sp->my_print();

    //Third Plugin    
    result = sp_win32_load_plugin(third_plugin, true);
    third_plugin_api *tp = (third_plugin_api *)sp_registry.get(THIRD_PLUGIN_API_NAME);
    tp->my_print();

    //Remove First Plugin
    sp_internal_api_registry_remove(PLUGIN_EXAMPLE_API_NAME);

    //Fourth Plugin
    result = sp_win32_load_plugin(fourth_plugin, true);
    fourth_plugin_api *fp = (fourth_plugin_api *)sp_registry.get(FOURTH_PLUGIN_API_NAME);
    fp->my_print();


    //Save the lastwrite time when we first load the plugin
    WIN32_FIND_DATA find_data = {};
    FindFirstFile(plugin_name,&find_data);
    FILETIME old_file_time = find_data.ftLastWriteTime;

    //my_print_func plugin_function = (my_print_func)GetProcAddress(plugin.handle,"my_print");

    //Loop forever
    while(1)
    {
        Sleep(1000);
        int modified = 0;
        //Win32 specific check to see if the file has been modified
        find_data = {};
        FindFirstFile(plugin_name,&find_data);
        FILETIME new_file_time = find_data.ftLastWriteTime;
        int changed = CompareFileTime(&old_file_time,&new_file_time);
        if(changed)
        {
            //They are not equal if we are here
            printf("plugin modified");
            //Reload the dll
            //FreeLibrary(plugin.handle);
            result = sp_win32_load_plugin(plugin_name, true);
            old_file_time = new_file_time;
            //Reload the procs
            plugin_example_api *pe = (plugin_example_api *)sp_registry.plugins[0].api;
            modified = 1;
        }
        else
        {
            printf("plugin not modified\n");
        }
        pe->my_print();
        if(modified)
        {
            //break;
        }
    }

    return 0;
}