//Inlude the header for the plugin
#include "second_plugin.h"

//Include anything else you need for the plugin
#include <stdio.h>


void my_second_print()
{
    printf("I am the second print. Hello there! \n");
}

SP_EXPORT void load_second_plugin(APIRegistry *reg, bool32 reload = false)
{
    printf("Second plugin is loaded!\n");

    SP_CREATE_API(second_plugin_api);
    SP_INIT_API_FUNC_PTR(second_plugin_api,my_second_print);
    SP_REGISTER_API(reg, second_plugin_api, reload);
}

SP_EXPORT void unload_second_plugin(APIRegistry *reg, bool32 reload)
{
    printf("I'll clean my stuff before being unloaded!\n");
    SP_REMOVE_API(registry, secpm_plugin_api, reload);
    //reg->remove(PLUGIN_EXAMPLE_API_NAME, reload);
    //reg->remove("sample_plugin_api", reload);
}


