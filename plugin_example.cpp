//------------------------------------------------------------------------------------------------------------
//This is an example of a plugin that can be used with the simple_plugin library.
//You can use this example a guide on how to write plugins or as a skeleton to be modified to your needs
//-------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------
//This is the c/cpp file for the plugin where the implementation lives:
//
// * Include the plugin header
//
// * There are two functions which are necessary to implement as they allow the simple_plugin client code
//   to interact with your plugin.
//   
//   First let's go trough the 'load' function, its signature and what to do inside of it.
//--------------------------------------------------------------------------------------------------------------

//- LOAD FUNCTION : CREATING THE API, INITIALIZING IT AND REGISTERING YOUR API WITH THE CLIENT CODE - 
//--------------------------------------------------------------------------------------------------------------------------------------
//   1)  The load function signature
//
//       SP_EXPORT void load_plugin_name(APIRegistry *reg, bool32 reload = false)
//    
//       This function is used by the client code to call into the plugin. You can do any kind of prep work here
//       that your plugin requires. But this function is mainly used to creating an instance of your API struct, initializing it
//       and then registering it with the client code.
//        
//       SP_EXPORT  - is a convenience macro that depending on your platform and compiler expands to give
//                    this function C linkage (extern "C") and exports the function (in msvc _declspec(dllexport) )
//
//       load_plugin_name - the name of the function should follow this convention, load_plugin_name where "plugin_name"
//                          is the same name as the plugin file (in this case sample_plugin.dll and this function load_sample_plugin )
//
//       APIRegistry *reg - as you can see from the function  we pass in a pointer to an APIRegistry 
//                          (called 'reg' here). The APIRegistry is an internal simple_plugin structure 
//                          that manages and keeps track of the plugins.
//       
//       bool32 reload - The 'reload' argument is used when a plugin is being reloaded, the default is set to false and you
//                       don't need to set it manually to true as the reloading is handled internally by simple_plugin.
//  
//       
//   2) Creating the API
//
//      SP_CREATE_API(api_struct_name) - this is a convenience macro that creates an instance of the API struct that you declare in the
//                                       header file. api_struct_name is the same name that you use when declaring the API struct in the header.
//                                       In this example that would be sample_plugin_name.
//    
//     SP_CREATE_API(sample_plugin_api) expands to  ->  static sample_plugin_api sample_plugin_api = {};
//     so it creates an instance of the sample_plugin_api struct, this instance is named sample_plugin_api.
//     
//    
//   3) Initializing the API
//      After we create the instance of the API struct we need to initialize the function pointers inside the struct
//      to point to their actual implementation inside of this c/cpp file.
//
//      SP_INIT_API_FUNC_PTR(api_struct_name,function_name) - this is another convenience macro that helps with initializing the function 
//                                                            pointers. 
//
//      api_struct_name is the name that we gave to the instance that initilzed with SP_CREATE_API. So the same api_struct_name we used in SP_CREATE_API(api_struct_name) 
//      
//      function_name is the name of the function and function pointer that we are initializing. The example below shows the following code :
//  
//       SP_INIT_API_FUNC_PTR(sample_plugin_api,my_print); -> so here we are initializing the function pointer my_print to point to an implementation
//                                                            also called my print. 
//      *** NOTE *** 
//      THIS IS ASSUMING THAT THE NAME OF THE FUNCTION POINTERS IN THE STRUCT ARE THE SAME AS THE NAME OF THE FUNCTION IMPLEMENTATION IN THE C/CPP FILE.
//      ***      ***
//
//      The macro expands to what is shown below, so in case you name your function pointers and function implementation differently you can just initialize
//      them manually.
//      
//      SP_INIT_API_FUNC_PTR(api_struct_name,function_name) api_struct_name.function_name = function_name 
//
//  
//
//   4) 
//-------------------------------------------------------------------------------------------------------------

//Inlude the header for the plugin
#include "plugin_example.h"

//Include anything else you need for the plugin
#include <stdio.h>

    
    void my_print()
    {
        printf("@@@@@@@  #######  @@@@@@@ \n");
    }

    void my_add_and_print(int a, int b)
    {
        printf("%d + %d  = %d \n", a,b, a + b);
    }



    //NECESSARY FUNCTION
    SP_EXPORT void load_plugin_example(APIRegistry *reg, bool32 reload = false)
    {
        printf("I have been loaded now I can do something\n");
        
        //Initing the api
        SP_CREATE_API(sample_plugin_api);

        //plugin_example_api.my_print = my_print;
        SP_INIT_API_FUNC_PTR(sample_plugin_api,my_print);
        SP_INIT_API_FUNC_PTR(sample_plugin_api,my_add_and_print);
        
        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        SP_REGISTER_API(reg, sample_plugin_api, reload);
        //reg->add(PLUGIN_EXAMPLE_API_NAME, &api)
    }

    //NECESSARY FUNCTION
    SP_EXPORT void unload_plugin_example(APIRegistry *reg, bool32 reload)
    {
        printf("I'll clean my stuff before being unloaded!\n");
        
        //Any clean up or any other stuff you want to do here...

        //These will be macroed so we don't have to remeber to add manually, it will be something more like
        
        reg->remove(PLUGIN_EXAMPLE_API_NAME, reload);
    }
   


}