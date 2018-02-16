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
//   1) The load_plugin_name function
//   2) The unload_plugin_name function.
//  
//   You can see more below about this two functions and how you should implement them.
//
// * In the rest of this file you should implement the functionality that your plugin provides.
//   Some of these functions will then be pointed too by the API struct function pointers that you declared
//   in the header file. More about this in the [Initializing the API] section below.
//--------------------------------------------------------------------------------------------------------------

//- [LOAD FUNCTION] : CREATING THE API, INITIALIZING IT AND REGISTERING YOUR API WITH THE CLIENT CODE - 
//--------------------------------------------------------------------------------------------------------------------------------------
//   1)  [The load function signature]
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
//       *** NOTE ***
//       THE NAME OF THE FUNCTION HAS TO FOLLOW THIS CONVENTION OTHERWISE THE CLIENT CODE WONT BE ABLE TO LOAD IT
//       ***      ***
//
//       load_plugin_name - the name of the function has to follow this convention, load_plugin_name where "plugin_name"
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
//   2) [Creating the API]
//
//      SP_CREATE_API(api_struct_name) - this is a convenience macro that creates an instance of the API struct that you declare in the
//                                       header file. api_struct_name is the same name that you use when declaring the API struct in the header.
//                                       In this example that would be sample_plugin_name.
//    
//     SP_CREATE_API(sample_plugin_api) expands to  ->  static sample_plugin_api sample_plugin_api = {};
//     so it creates an instance of the sample_plugin_api struct, this instance is named sample_plugin_api.
//     
//    
//   3) [Initializing the API]
//
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
//   4) [Registering the API with the client code]
//      
//      The last thing that we must do is to register the plugin (and it's API) with the client code.
//      As was stated above the APIRegistry is a structure of the simple_plugin library which is responsible
//      for managing and keeping track of plugins for the client code.
//
//      SP_REGISTER_API(registry,api_struct_name,reload) - this is another convenience macro that registers our API struct with the
//                                                         passed in registry.
//
//      registry - the APIRegistry *reg that was passed in to the load function
//
//      api_struct_name - the name of the API struct instance that we created with SP_CREATE_API.
//                        this would be the same api_struct_name that was used in SP_CREATE_API and SP_INIT_API_FUNC_PTR
//
//     reload - the bool32 reload argument that was passed in the load function.
//
//     This macro expands to the following :
//
//     SP_REGISTER_API(registry,api_struct_name,reload) reg->add(#api_struct_name,&api_struct_name,reload, reg)
//
//     As you can see we are just calling the APIRegistry add method, passing in the api_struct_name as a string,
//     (this is hashed and used internally to id the API), the address of the API structure instance we created and the 
//     reload variable.
//-------------------------------------------------------------------------------------------------------------


//- [UNLOAD FUNCTION] : - 
//--------------------------------------------------------------------------------------------------------------------------------------
//   1)  This function is used to allow the plugin to do any clean-up it might want to.
//       It is also used to signal to the client code to remove this plugin and API from the registry.
//       
//       The unload function signature follows the same convention as the load function.
//       The ONLY difference is that this function is called unload_plugin_name.
//
//       SP_EXPORT void unload_plugin_example(APIRegistry *reg, bool32 reload)
//
//       *** NOTE ***
//       THE NAME OF THE FUNCTION HAS TO FOLLOW THIS CONVENTION OTHERWISE THE CLIENT CODE WONT BE ABLE TO LOAD IT
//       ***      ***
//
//       load_plugin_name - the name of the function has to follow this convention, unload_plugin_name where "plugin_name"
//                          is the same name as the plugin file (in this case sample_plugin.dll and this function unload_sample_plugin )
//
//       The other parts of the function signature were covered in the LOAD FUNCTION section.
//
//   2)  [Removing the API from the registry]
//       
//       We use the following macro to tell the APIRegistry to remove us from the system.
//
//       SP_REMOVE_API(registry, api_struct_name, reload) 
//      
//       registry - the APIRegistry *reg that was passed in as an argument to the unload function.
// 
//       api_struct_name - the name of the API struct.
//  
//       reload          - the bool32 reaload argument that was passed into the unload function.
//
//       The macro expands to the following:
//
//       SP_REMOVE_API(registry, api_struct_name, reload) reg->remove(#api_struct_name, reload, reg);
//       
//       As you can see all we are doing is calling the remove method of the APIRegistry.
//
//
//--------------------------------------------------------------------------------------------------------------------------------------


//Inlude the header for the plugin
#include "sample_plugin.h"

//Include anything else you need for the plugin
#include <stdio.h>

//---
//Implementations for the functionality that the plugin provides.
//
//This can be as as simple or complex as you want it to be. 
//If you are using the helper macros SP_INIT_API_FUNC_PTR() in the load_plugin_name function then you need to make sure that 
//the name of the implementation functions is the same as the name of the function pointer declared in the header.
//
//For this sample plugin the API struct was declared as such :
//
//
//      struct sample_plugin_api
//      {
//           //These two are equivalent
//           //void (*my_print)();
//           SP_API_FUNCTION(void, my_print, () );
//
//          //These two are equivalent
//          //void (*my_add_and_print)(int,int);
//          SP_API_FUNCTION(void, my_add_and_print, (int,int) );
//      };
//
//You can see how we used the same names for the function pointers (int the struct) and
//for the functions which these pointers will point too (in this file ).
//
//
//If you are NOT using the helper macro SP_INIT_API_FUNC_PTR() then you can name them differnetly as long
//as you init them manually. 
//In this case you would do something like
//
//         sample_plugin.api.my_print = my_print_with_different_name;
//
//---
void my_print()
{
    printf("@@@@@@@  #######  @@@@@@@ \n");
}

void my_add_and_print(int a, int b)
{
    printf("%d + %d  = %d \n", a,b, a + b);
}



//---
//Load function for this plugin , you MUST implement this function and do the following:
//1)Create API   - SP_CREATE_API
//2)Init API     - SP_INI_API_FUNC_PTR  for all the functions the API provides
//3)Register API - SP_REGISTER_API  
//
//You can do any other initilization or prep code you feel like in this function.
SP_EXPORT void load_plugin_sample(APIRegistry *reg, bool32 reload = false)
{
    printf("I have been loaded now I can do something\n");

    //Creating the API

    //static sample_plugin_api sample_plugin_api = {};
    SP_CREATE_API(sample_plugin_api);


    //Initializing the API

    //These two are equivalent
    //sample_plugin_api.my_print = my_print;
    SP_INIT_API_FUNC_PTR(sample_plugin_api,my_print);

    SP_INIT_API_FUNC_PTR(sample_plugin_api,my_add_and_print);
    
    //Registering the API
    SP_REGISTER_API(reg, sample_plugin_api, reload);
    //reg->add("sample_plugin_api", &api, reload, reg);
}

//---
//Unload function for this plugin, you MUST implement this function and do the following:
//1)Remove from the client code - SP_REMOVE_API
//
//You can do any other clean-up in this function.
SP_EXPORT void unload_plugin_sample(APIRegistry *reg, bool32 reload)
{
    printf("I'll clean my stuff before being unloaded!\n");
    
    //Any clean up or any other stuff you want to do here...

    SP_REMOVE_API(reg, sample_plugin_api, reload);
    //reg->remove(PLUGIN_EXAMPLE_API_NAME, reload,reg);
    //reg->remove("sample_plugin_api", reload, reg);
}


