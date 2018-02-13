//This is the header file for the plugin here you specify the following:

//The Plugin Api Name - this is the name that will be quieried in client code, when a client app wants to use the api
//The API             - inside a struct whose name will be the same as the string for the api name.


//This will be macroed into somethin like
// SP_API_NAME(name)
#define THIRD_PLUGIN_API_NAME "third_plugin_api"

struct third_plugin_api
{
    void (*my_print)();

};
