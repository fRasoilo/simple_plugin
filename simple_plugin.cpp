//Example of host loading a plugin

#include <Windows.h>

#include <stdio.h>

#include "plugin_example.h"

#include "simple_plugin.h"

char* plugin_name = "plugin_example.dll";
char* second_plugin = "second_plugin.dll";

int main()
{

    SPlugin plugin = {};
    bool32 result = sp_win32_load_plugin(plugin_name, true);
    result = sp_win32_load_plugin(second_plugin, true);

    //Save the lastwrite time when we first load the plugin
    WIN32_FIND_DATA find_data = {};
    FindFirstFile(plugin_name,&find_data);
    FILETIME old_file_time = find_data.ftLastWriteTime;

    my_print_func plugin_function = (my_print_func)GetProcAddress(plugin.handle,"my_print");

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
            FreeLibrary(plugin.handle);
            result = sp_win32_load_plugin(plugin_name, true);
            old_file_time = new_file_time;
            //Reload the procs
            plugin_function = (my_print_func)GetProcAddress(plugin.handle,"my_print");
            modified = 1;
        }
        else
        {
            printf("plugin not modified\n");
        }
        plugin_function();
        if(modified)
        {
            //break;
        }
    }

    return 0;
}