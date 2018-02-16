
#include "simple_plugin.h"

//Define our API NAME -- this will be used in client code to load the API
#define SECOND_PLUGIN_API_NAME "second_plugin_api"

//Declare the "API struct" and the function pointers
//REMENBER: the name of this struct MUST be the same name that API_NAME above resolves too, since this will be used in client code
//to load the API. 
struct second_plugin_api
{
    //These two are equivalent
    //void (*my_print)();
    SP_API_FUNCTION(void, my_second_print, () );

};

//Keep any global or heap allocated state here, this can be used if we want to transfer
//state between two plugins of the same kind. For example in hot reloading.
struct plugin_state
{

};