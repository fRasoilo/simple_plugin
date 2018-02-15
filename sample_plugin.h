//------------------------------------------------------------------------------------------------------------
//This is an example of a plugin that can be used with the simple_plugin library.
//You can use this example a guide on how to write plugins or as a skeleton to be modified to your needs
//-------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------
//This is the header file for the plugin here you specify the following:
//
// * Include simple_plugin.h
//
// * Define the plugin api name -  this is the name that will be quieried in client code, when a client app wants to use the api
//      * The convention is to use #define PLUGIN_NAME_API_NAME "plugin_name_api"
//          * Where PLUGIN_NAME is the name of the plugin file, in this case the plugin is sample_plugin.dll so we use SAMPLE_PLUGIN_API_NAME
//            and "sample_plugin_api"
//
// * Declare the api - inside a struct whose name will be the same as the string for the api name.
//      * The convention here is to declare a struct whose name is plugin_name_api , so in this case the struct will be sample_plugin_api
//        this makes it so that we can easily find and load this struct by asking for our defined api name.
//      * The name of this struct MUST be the same name that PLUGIN_NAME_API_NAME resolves too, this is because this will be the 
//        name that client code will use to load the API.
//
//-------------------------------------------------------------------------------------------------------------
 
//-------------------------------------------------------------------------------------------------------------
// Declaring the api
//
// 
// So out "api struct" gives us a quick overview of the API of the plugin.
// Inside this struct we have function pointers to all the functions that this plugin provides.
// The reason for specifying the API in this way is so that it is much easier to load the plugin
// instead of having to load all the different functions that are implemented we simply load this
// API struct , take a look at the header file to see the API and what functions we are interested in
// and call them from the pointers provided by the "API struct".
// 
//
//  * simple.plugin.h provides some helpfull macros for specifying the function pointers in the API struct.
//    You can use SP_API_FUNCTION(return_type, function_name, (params) ) to specify the function pointers.
//    All this macro is doing is making a function pointer as such : return_type (*function_name)(params)
//    It only acts as syntactic sugar and if you feel prefer to just specify the function pointers directly feel free to do so. 
//-------------------------------------------------------------------------------------------------------------
 

#include "simple_plugin.h"

//Define our API NAME -- this will be used in client code to load the API
#define SAMPLE_PLUGIN_API_NAME "sample_plugin_api"

//Declare the "API struct" and the function pointers
//REMENBER: the name of this struct MUST be the same name that API_NAME above resolves too, since this will be used in client code
//to load the API. 
struct sample_plugin_api
{
    //These two are equivalent
    //void (*my_print)();
    SP_API_FUNCTION(void, my_print, () );

    //These two are equivalent
    //void (*my_add_and_print)(int,int);
    SP_API_FUNCTION(void, my_add_and_print, (int,int) );
};

//Keep any global or heap allocated state here, this can be used if we want to transfer
//state between two plugins of the same kind. For example in hot reloading.
struct plugin_state
{

};