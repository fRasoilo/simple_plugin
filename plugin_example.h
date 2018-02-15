#include "simple_plugin.h"

//This is the header file for the plugin here you specify the following:

//The Plugin Api Name - this is the name that will be quieried in client code, when a client app wants to use the api
//The API             - inside a struct whose name will be the same as the string for the api name.


//This will be macroed into somethin like
// SP_API_NAME(name)
#define PLUGIN_EXAMPLE_API_NAME "plugin_example_api"

struct plugin_example_api
{
    //void (*my_print)();
    SP_API_FUNCTION(void, my_print, () );
    SP_API_FUNCTION(void, my_add_and_print, (int,int) );
};

//Keep any global or heap allocated state here, this can be used if we want to transfer
//state between two plugins of the same kind. For example in hot reloading.
struct plugin_state
{

};