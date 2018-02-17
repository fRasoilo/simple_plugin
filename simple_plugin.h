#pragma once


//Simple Plugin --------------

//=============================================================================
// Summary : 
//
//  simple_plugin is a small library written in C++ that provides an easy way to
//  add plugins to an application. simple_plugin also support Hot-Reloading of plugins
//  This means you can have faster iteration times when working: make a change, compile,
//  simple_plugin hot reloads any plugin you have set to be reloadable instantly.
//
//  It is a single file header that should be very easy to drop-in and start using in any pre-existing project.
//=============================================================================
// Revision History :
//
//
//===============================================================================  
// Todo List :
// 
//  - Support for differnt OS.  
//  - Be able to inform the user exactly which plugin has been modified when hot-reloading.
//  - Make so that pointers to plugins do NOT become invalid when the plugin is hot reloaded.
//  - Make it so that we can specify if a the registry is dynamic or static.
//  - Be able to specify the hash function used.
//  
//
//===============================================================================  
//  You MUST define SIMPLE_PLUGIN_IMPLEMENTATION
//
//  in exactly *one* C or C++ file that includes this header, BEFORE the include
//  like so:
//
//  #define SIMPLE_PLUGIN_IMPLEMENTATION
//  #include "simple_plugin.h"
//
//  All other files should just include "simple_plugin.h" without the define
//=============================================================================
// Basic Usage / Quick Start :
//
// After defining SIMPLE_PLUGIN_IMPLEMENTATION
//
// ** In the CLIENT code **
//
// call     sp_load_plugin(char* plugin_name, bool32 reloadable);
// where        plugin_name is the name of a plugin (dll) you have created
//              realoadable indicates whether this plugin should be hot-reloaded if changed
//
//
// use       sp_get_api(char* api_name) to get back a pointer to the API you request.
//              you must cast this pointer to the appropriate type that represents the API struct
//              you are requesting, this information can be found in the plugin header.           
//
// FOR A COMPLETE EXAMPLE, CHECK OUT simple_plugin.cpp where there is a small but comprehensive
// program that shows how to use the library
//
//
// ** Creating Plugins **
//
// For a guide on how to create plugins, please check out the sample_plugin.h and sample_plugin.cpp
// files.
// These two files show a complete example plugin with comprehensive comments explaining how to create a plugin,
// the conventions used and what functions from the library you need to use.
// You can either use the sample plugin as a guide or as a skeleton for writing your own.
//
//
//  ** ATTENTION FOR WINDOWS USERS **
//  --------------------------------
//  There is a well known issue with Visual Studio locking pdb files for executables.
//  This affects hot-reloading if using Visual Studio (for debugging or otherwise) since the 
//  pdb will be locked and we wont be able to recompile our plugins (dlls).
//
//  There are different solutions out there for dealing with this, one of which is to 
//  give the pdb a unique name when we compile our plugins.
//
//  If using the msvc compiler from the command line you can use the following switch to set the
//  pdb file to a random number or add a random number to another string.
//
//  Example of compiling a dll with msvc command line tool and setting pdb to random number
// 
//  cl /LD sample_plugin.cpp /PDB:sample_plugin.%RANDOM%.pdb
//
//  I have not tried to do this in Visual Studio as I usually compile from the command line,
//  however this answer seems to have worked for people:
//
//  https://stackoverflow.com/questions/36387105/how-do-you-in-visual-studio-generate-a-pdb-file-with-a-random-outputted-name#36447979
//   
//
//  If you have other solutions please feel free to get in touch
//
//  @filipe_rasoilo
//  rasoilo.com
//  *********************************
//===============================================================================
//
// API  : API reference can be found further down.
//         [Loading a plugin]
//         [Unloading a plugin]
//         [Querying for an API / Getting an API]
//         [Hot Reloading Plugins]
//         [Creating another API registry and destroying it]  
//===============================================================================  


// [INTERNAL] Types --------------
#include <stdint.h>
//Unsigned
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t  uint8;
//Signed
typedef int64_t int64;
typedef int32_t int32;
//Bool
typedef int32 bool32;

// [INTERNAL] Static Declarations
#define internal        static
#define global_variable static
#define local_persist   static


#define SP_Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

// [INTERNAL] Utility Macros
#define Bytes(n)  (n)
#define KiloBytes(n)  (Bytes(n)*1024)
#define InvalidCodePath SP_Assert(!"InvalidCodePath")
#define InvalidDefaultCase default: {InvalidCodePath;} break

//@NOTE: Helper macros to be used in the creation of plugins
#define SP_CREATE_API(api_struct_name) internal api_struct_name api_struct_name = {}
#define SP_INIT_API_FUNC_PTR(api_struct_name,function_name) api_struct_name.function_name = function_name 
#define SP_REGISTER_API(reg,api_struct_name,reload) reg->add(#api_struct_name,&api_struct_name,reload, reg)
#define SP_REMOVE_API(reg, api_struct_name, reload) reg->remove(#api_struct_name, reload,reg);
#define SP_API_FUNCTION(return_type, function_name, params) return_type (*function_name) params 

//SP_EXPORT
#ifdef _WIN32
    #ifdef __cplusplus
        #define SP_EXPORT extern "C" __declspec(dllexport)
    #else
        #define SP_EXPORT __declspec(dllexport)
    #endif //__cpluscplus
#else
    //@TODO: Other OS
    #error No other OS defined
#endif //_WIN32



//[INTERNAL] -- This is not really part of the API but it is defined here so that the plugins can see it without having to drag int
//              the rest of the library.
//API registry -- idea taken from "http://ourmachinery.com/post/little-machines-working-together-part-1/"

//CAN BE USER DEFINED
#define SP_REGISTRY_INITIAL_CAPACITY 10
#define SP_REGISTRY_GROWTH_FACTOR    2
#define SP_MAX_PLUGINS 100

//forward declare
struct SPlugin;

struct APIRegistry
{
    int32 capacity;
    int32 used;

    SPlugin *plugins;

    SPlugin *reloadable_plugins[SP_MAX_PLUGINS];
    uint16 reloadable_count;
    //Some helpers to speed up lookup of plugins and "holes".
    SPlugin *curr;
    int32 next_hole_index;

    void (*add)(char* plugin_name, void* api, bool32 reload, APIRegistry* registry);
    void (*remove)(char* plugin_name, bool32 reload, APIRegistry *registry);
    void* (*get)(uint64 api_hash, APIRegistry *registry);


};

//=============================================================================
// API - [Loading a plugin]
//
//=============================================================================

//Used to load in a plugin.
//plugin_name - specifies name of the plugin
//reloadable  - this indicates whether we want to hot reload this plugin in case of recompile
//
//This function returns a pointer to the plugin structure that was created by the library and registred with the
//default  API registry sp_registry. This is a static variable of APIRegistry that the library creates and use as the 
//default registry to keep track of plugins. However a user of the library has the option of creating their own registry, 
//and many functions take a registry as an argument.
//The user has the option of using this plugin pointer directly to query for the API, however it is
//not necessary to keep this pointer as there are other functions available to query a plugin's API.
//
// *** ATTENTION ***
// At the moment it is NOT advisable to keep direct pointers to plugins that will be hot-reloaded as these pointers
// may become invalid once the plugin is hot-reload by the library. This might be fixed in the future. @TODO.
SPlugin * sp_load_plugin(char* plugin_name, bool32 reloadable);

//This function is the same as the one above, however in here we can pass in a pointer to a APIRegistry that we have created.
//The loaded plugin will be registred with the registry that we pass in (and not the default user created registry as above).
//For an example of this, please refer to the simple_plugin.cpp file.
SPlugin * sp_load_plugin(APIRegistry *registry, char* plugin_name, bool32 reloadable);

//=============================================================================
// API - [Unloading a plugin]
//
//=============================================================================

//All these functions unload a plugin from the system, what this means is that
//we call the user specified unload function of the plugin. 
//After that we remove the plugin from whichever APIRegistry was keeping track of it.
//And we clean up by closing any handles to files or libraries that might be left.

//Unload a plugin from a given registry by using the api_name that the plugin offers.
//This would be the PLUGIN_NAME_API_NAME that is the defined in the plugin header.
//See the sample_plugin.h file for more details on what the PLUGIN_NAME_API_NAME define refers too.
//
//registry - the registry to which this plugin belongs too.
//api_name - the api_name whose plugin we want to unload. 
void sp_unload_plugin(APIRegistry * registry,char* api_name);

//This is the same as above, however in this one we pass in a pointer to the plugin directly.
//registry - the registry to which this plugin belongs too.
//plugin   - a pointer to a SPLugin that we want to unload.
void sp_unload_plugin(APIRegistry * registry,SPlugin *plugin);

//The same as sp_unload_plugin(APIRegistry * registry,char* api_name);
//But in this function the registry is the default registry that is created by the library.
void sp_unload_plugin(char* api_name);

//The same as void sp_unload_plugin(APIRegistry * registry,SPlugin *plugin);
//However in this instance the registry is the default registry that is created by the libary 
void sp_unload_plugin(SPlugin *plugin);

//
//=============================================================================
// API - [Querying for an API / Getting an API]
//
//=============================================================================

//This function returns a void pointer that points to the API struct of a given plugin.
//We have to cast this pointer to the appropriate struct type so we can use the API provided by a plugin.
//All this is available in the plugins header.
//
//plugin - pointer to a SPlugin 
void * sp_get_api(SPlugin *plugin);

//Same as above only in this function we are passing in the name of the API that is presented by the plugin.
//api_name - name of the API made available by the plugin (convention is PLUGIN_NAME_API_NAME)
void * sp_get_api(char *api_name);

//The next two functions act exactly the same as the ones above, only difference is that we are passing in a different APIRegistry,
//instead of using the default.
//If a plugin is registred with a different registry then we MUST use these versions and pass in the correct registry.
void * sp_get_api(APIRegistry *registry,SPlugin *plugin);

//
void * sp_get_api(APIRegistry *registry,char *api_name);


//=============================================================================
// API - [Hot Reloading Plugins]
//
//=============================================================================

//This function is used tell the API registry to scan all the plugins that are registred as 'reloadable'.
//If any of these plugins have been changed then the library is in charge of loading the new version, unloading the old version
//and registring thhe new version appropriatly.
//
//***ATTENTION***
// - Any pointers that you might have had that point directly to a plugin might become invalid for hot-reloadable plugins 
// - While the library is in charge of monitoring the plugin, loading the new one and unloading the old version, the USER
//   is responsible for getting the API from the new version.
// - Check the simple_plugin.cpp file for more details.
//
//The return type is a bool32 that indicated whether any plugins have changed, tho at the moment the library does not specify which one. @TODO
bool32 sp_update();

//Same as above but instead of updating the default registry we update the given registry
//registry - a user specified registry
bool32 sp_update(APIRegistry *registry);

//=============================================================================
// API - [Creating another API registry and destroying it]
//
//=============================================================================

//This function creates an api registry that is user defined. 
//In the simple_plugin.cpp file you can see an example of how or why a user might do this.
//
// capacity - the initial capacity of the registry.
// 
// The function returns the created APIRegistry struct.
APIRegistry sp_registry_create(uint32 capacity);

//This function is used to destroy a user created registry.
//All plugins will be removed from the registry, all plugins will call their unload function and any file or library handles will be cleaned up.
void sp_registry_destroy(APIRegistry *registry);



//===============================================================================  
// [IMPLEMENTATION]
//=============================================================================== 
#ifdef SIMPLE_PLUGIN_IMPLEMENTATION

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h> //malloc, realloc
#endif //_WIN32

#include <stdio.h>

// String utilities ----------------------------------------------------

// [INTERNAL] Str Buffer
#define STR_BUFFER_SIZE KiloBytes(1)
struct StrBuffer
{
    char buffer[STR_BUFFER_SIZE];
    uint32 used;
};

global_variable StrBuffer str_buffer_; //TODO: Is this necessary?
//

inline  int32
sp_string_len(char* string)
{
    int32 count = 0;
    while(*string++)
    {
        ++count;
    }
    return(count);
}

inline int32
sp_string_size(char* string)
{
    int32 length = sp_string_len(string);

    int32 size = length / sizeof(string[0]);
    return(size);
}

internal void
sp_buffer_append_string(StrBuffer* str_buffer, char* string)
{
    uint32 string_size = sp_string_size(string);
    SP_Assert((string_size + str_buffer->used) < STR_BUFFER_SIZE); 
    char* str_ptr = string;
    for(int32 index = str_buffer->used ;*str_ptr; ++index)
    {
        str_buffer->buffer[index] = *str_ptr++;
    }
    str_buffer->used += string_size;

    str_buffer->buffer[str_buffer->used] = '\0';    
}

internal void
sp_buffer_append_char(StrBuffer* str_buffer, char c)
{
    uint32 char_size = sizeof(c);                             
    SP_Assert((char_size + str_buffer->used) < STR_BUFFER_SIZE); 
    str_buffer->buffer[str_buffer->used] = c;
    str_buffer->used += char_size;
    str_buffer->buffer[str_buffer->used] = '\0';    
}

internal void
sp_buffer_append_newline(StrBuffer* str_buffer)
{
    uint32 string_size = sizeof('\n');                             
    SP_Assert((string_size + str_buffer->used) < STR_BUFFER_SIZE); 
    str_buffer->buffer[str_buffer->used] = '\n';
    str_buffer->used += string_size;
    str_buffer->buffer[str_buffer->used] = '\0';    
}

internal int32
sp_internal_print_to_buffer(StrBuffer* str_buffer, char* fmt,va_list args)
{
    //NOTE: Becasue vsprintf wont respect if there is already something in the buffer, we use a temp_buffer so we dont override the original.
    StrBuffer temp_buffer = {};
    int32 result = vsprintf(temp_buffer.buffer, fmt,args);
    sp_buffer_append_string(str_buffer, temp_buffer.buffer);
    return(result);
}

internal int32
sp_print_to_buffer(StrBuffer* str_buffer, char* fmt,...)
{
    va_list args;
    va_start(args, fmt);
    int32 result = sp_internal_print_to_buffer(str_buffer, fmt, args);
    va_end(args);

    return(result);
}

inline int32
sp_string_size_strip_extension(char* string)
{
    char* c = string;
    int32 len = 0;
    while(*c++)
    {
        if(*c == '.')
        {
            break;
        }
        ++len;
    }
    int32 size = len / sizeof(string[0]);
    return(size);
}


inline void
sp_string_build_load_function_name(char* plugin_name, StrBuffer *buffer)
{
    char* c = plugin_name;
    sp_print_to_buffer(buffer, "load_");

    while(*c)
    {
        if(*c == '.')
        {
            sp_buffer_append_char(buffer,'\0');
            break;
        }
        sp_buffer_append_char(buffer, *c);
        c++;
    }
}

inline void
sp_string_build_unload_function_name(char* plugin_name, StrBuffer *buffer)
{
    char* c = plugin_name;
    sp_print_to_buffer(buffer, "unload_");

    while(*c)
    {
        if(*c == '.')
        {
            sp_buffer_append_char(buffer,'\0');
            break;
        }
        sp_buffer_append_char(buffer, *c);
        c++;
    }
}

inline void
sp_string_build_tmp_name(char* plugin_name, StrBuffer *buffer, int32 count = 0)
{
    
    char *c = plugin_name;
    
    while(*c)
    {
        if(*c == '.')
        {
            sp_print_to_buffer(buffer, "_temp%d", count);
        }
        sp_buffer_append_char(buffer, *c);
        ++c;
    }
}

//Given a full path to the plugin, we extract the name
inline void 
sp_string_extract_plugin_name(char* plugin_full_path, char* extracted_name)
{
    char* c = plugin_full_path;
    char* last_slash = c;
    //Find the last slash
    while(*c)
    {
        if(*c == '\\'){
            last_slash = c;
        }
        ++c;
    }
    //go one after the last slash
    c = ++last_slash;

    //copy to extracted name
    char* t = extracted_name;
    while(*c)
    {
        *t++ = *c++;
    }
    *t = '\0';
}


// End String Utilities

struct SPlugin
{
    uint64 hash;
    uint64 api_hash;
    uint32 reload_count;
    bool32 reloadable;

    void* api;
    void* unload_func;

    //Win32 Specific
    HMODULE library_handle;
    HANDLE file_handle;
    FILETIME last_write_time;

};

bool32 sp_plugin_is_initialized(SPlugin* plugin)
{
    //Plugin should NOT be considered initialzed if any of these are Zero or null.
    return( plugin->hash && plugin->api_hash && plugin->api);
}

//Hash Functions ----------------------------------------------------
//@TODO: Try out MurmurHash3

#define SP_HASH(string) sp_internal_djb2_hash(string)

// djb2 Taken from  "http://www.cse.yorku.ca/~oz/hash.html"
unsigned long sp_internal_djb2_hash(char* str)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }    
    return (hash);
}
//End Hash Functions ----------------------------------------------------






//Typedefs of the plugin load and unload function signatures
typedef void (*load_func)(APIRegistry *, bool32);
typedef void (*unload_func)(APIRegistry *, bool32);


void sp_internal_api_registry_add(char* api_name, void* api, bool32 reload, APIRegistry *registry);
void sp_internal_api_registry_remove(char* plugin_name, bool32 reload, APIRegistry *registry);
void * sp_internal_api_registry_get(uint64 api_hash, APIRegistry *registry);
void sp_internal_api_registry_transfer_state(void* old_state, void* new_sate);



internal APIRegistry sp_internal_registry_create()
{
    APIRegistry reg = {};
    reg.capacity        = SP_REGISTRY_INITIAL_CAPACITY;
    reg.used            = 0;
    reg.plugins         = (SPlugin*)malloc(sizeof(SPlugin) * reg.capacity);
    memset(reg.plugins,0,sizeof(SPlugin) * reg.capacity); //set all plugin values to zero.
    //reg.reloadable_indexes = 0;
    reg.reloadable_count = 0;
    reg.curr            = reg.plugins;
    reg.next_hole_index = 0;
    reg.add             = sp_internal_api_registry_add;
    reg.remove          = sp_internal_api_registry_remove;
    reg.get             = sp_internal_api_registry_get;
    return(reg);
}

global_variable APIRegistry sp_registry = sp_internal_registry_create();

APIRegistry sp_registry_create(uint32 capacity)
{
    APIRegistry reg = {};
    reg.capacity        = capacity;
    reg.used            = 0;
    reg.plugins         = (SPlugin*)malloc(sizeof(SPlugin) * reg.capacity);
    memset(reg.plugins,0,sizeof(SPlugin) * reg.capacity); //set all plugin values to zero.
    //reg.reloadable_indexes = 0;
    reg.reloadable_count = 0;
    reg.curr            = reg.plugins;
    reg.next_hole_index = 0;
    reg.add             = sp_internal_api_registry_add;
    reg.remove          = sp_internal_api_registry_remove;
    reg.get             = sp_internal_api_registry_get;
    return(reg);
}

internal APIRegistry* 
sp_internal_registry_get()
{
    return (&sp_registry);
}

void sp_internal_api_registry_add(char* api_name, void* api, bool32 reload, APIRegistry *registry)
{
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }
    SPlugin *plugin = nullptr;

    plugin = reg->curr;
    plugin->api_hash = SP_HASH(api_name);
    plugin->api = api;

    reg->used++;

    
    if(reg->used < reg->capacity) 
    {
        //@TODO: This seems unnecessary....
        if((reg->next_hole_index - 1) < reg->used /*0 based indexes*/)
        {
            //scan to find the next position
            uint32 count = reg->capacity;
            for(uint32 index = 0; index < count; ++index )
            {
                plugin = &reg->plugins[index];
                if(!sp_plugin_is_initialized(plugin))
                {
                    reg->curr = plugin;
                    break;
                }
            }
        }
        else
        {
            reg->curr++;
        }
    }
}

void sp_internal_api_registry_remove_reloaded(uint64 hash, APIRegistry* registry)
{
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }
    //@NOTE: If we are removing a plugin that was reloaded we assume there is still another one here.

    SPlugin *plugin = reg->plugins;
    SPlugin *first_found  = nullptr;
    SPlugin *second_found = nullptr;
    
        
    uint32 count   = reg->capacity; 
    for(uint32 index = 0; index < count; ++index )
    {
        plugin = &reg->plugins[index];
        if(hash == plugin->api_hash)
        {
            if(!first_found)
            {
                first_found = plugin;
                continue;
            }
            else
            {
                second_found = plugin;
            }

            plugin = first_found->reload_count < second_found->reload_count ? first_found : second_found;
            *plugin = {}; //reset this slot
            reg->curr = plugin;
            break;
        }
    }

}

void sp_internal_plugin_cleanup(SPlugin *plugin)
{
    #ifdef _WIN32
        CloseHandle(plugin->file_handle);
        FreeLibrary(plugin->library_handle);
    #else
        //@TODO: Do other OS
        #error NO OTHER OS DEFINED

    #endif //_WIN32
}

void sp_internal_api_registry_remove(char* api_name, bool32 reload, APIRegistry *registry)
{
    //@TODO: Make sure to close the file_handle (win32) if this was a reloadable plugin.
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }

    uint64 desired_api_hash = SP_HASH(api_name);

    if(reload)
    {
        sp_internal_api_registry_remove_reloaded(desired_api_hash, reg);
    }
    else
    {
        SPlugin *plugin = reg->plugins;
        uint32 count   = reg->capacity; 
        for(uint32 index = 0; index < count; ++index )
        {
            plugin = &reg->plugins[index];
            if(desired_api_hash == plugin->api_hash)
            {
                reg->curr = &reg->plugins[index];
                break;
            }
        }
    }
    reg->used--;
}

void * sp_internal_api_registry_get(uint64 api_hash, APIRegistry *registry)
{
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }

    uint64 desired_api_hash = api_hash;
    SPlugin *plugin = reg->plugins;
    uint32 count   = reg->capacity; 
    for(uint32 index = 0; index < count; ++index )
    {
        plugin = &reg->plugins[index];
        if(desired_api_hash == plugin->api_hash)
        {
            return(plugin->api);
        }

    }
    return(0);
}

void * sp_get_api(APIRegistry *registry,char *api_name)
{
    uint64 api_hash = SP_HASH(api_name);
    return(sp_internal_api_registry_get(api_hash, registry));
}

void * sp_get_api(APIRegistry *registry,SPlugin *plugin)
{
    return(sp_internal_api_registry_get(plugin->api_hash, registry));
}

void * sp_get_api(char *api_name)
{
    uint64 api_hash = SP_HASH(api_name);
    return(sp_internal_api_registry_get(api_hash, nullptr));
}

void * sp_get_api(SPlugin *plugin)
{
    return(sp_internal_api_registry_get(plugin->api_hash, nullptr));
}



//Used to add a new plugin to the registry, if there is enough space all it does is return a 
//pointer to the curr plugin. If there is not enough space then we will reallocate, copy the old memory
//and then return a pointer to the new curr.
SPlugin* sp_internal_api_registry_add_new_plugin(APIRegistry *registry)
{
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }

    if(reg->used < reg->capacity)
    {
        return(reg->curr);
    }
    else
    {
        int32 old_capacity = reg->capacity;
        reg->capacity = old_capacity*SP_REGISTRY_GROWTH_FACTOR;

        void* alloc_memory = realloc(reg->plugins,sizeof(SPlugin) * reg->capacity);
        if(!alloc_memory)
        {
            SP_Assert(!"Could not reallocate block");
        }
        reg->plugins = (SPlugin *)alloc_memory;
        size_t new_chunk_byte_size = sizeof(SPlugin) * (reg->capacity - old_capacity);
        memset(reg->plugins + old_capacity, 0, new_chunk_byte_size);

        reg->curr = reg->plugins + reg->used;

        return(reg->curr);
    }
}

bool32 sp_internal_win32_plugin_modified(SPlugin *plugin)
{
    FILETIME last_write_time = {};
    GetFileTime(plugin->file_handle,0,0,&last_write_time);
    
    bool32 modified = CompareFileTime(&plugin->last_write_time,&last_write_time);
    if(modified)
    {
        plugin->last_write_time = last_write_time;
    }

    return(modified);
}

bool32 sp_internal_plugin_modified(SPlugin *plugin)
{
    
    #ifdef _WIN32
        bool32 result = sp_internal_win32_plugin_modified(plugin);
    #else   
        //@TODO: Other OS
        #error NO OTHER OS DEFINED    
    #endif //_WIN32

    return(result);
}

//Forward declare.
bool32 sp_internal_win32_reload_plugin(SPlugin* plugin, int32 index, APIRegistry *registry);


bool32 sp_internal_reload_plugin(SPlugin* plugin, int32 index, APIRegistry *registry)
{
     #ifdef _WIN32
        bool32 result = sp_internal_win32_reload_plugin(plugin, index, registry);
    #else   
        //@TODO: Other OS
        #error NO OTHER OS DEFINED    
    #endif //_WIN32

    return(result);
}

bool32 sp_internal_api_registry_check_reloadable_plugins(APIRegistry *registry)
{
    bool32 result = false;

    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }

    uint32 count = reg->reloadable_count;
    for(uint32 index = 0; index < count; ++index)
    {
        if(sp_internal_plugin_modified(reg->reloadable_plugins[index]))
        {
            printf("Plugin at index : %d has been modified!\n", index);
            SPlugin *plugin = reg->reloadable_plugins[index];
    
            result = sp_internal_reload_plugin(plugin, index, reg);
            
        }
    }
    return(result);
}


bool32 sp_update(APIRegistry *registry)
{
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }
    bool32 result = sp_internal_api_registry_check_reloadable_plugins(reg);

    return(result);
}

bool32 sp_update()
{
    return(sp_update(nullptr));
}

void sp_registry_destroy(APIRegistry *registry)
{
    if(!registry)
    {
        registry = sp_internal_registry_get();
    }
    uint32 count = registry->capacity;
    for(uint32 index = 0; index < count; ++index)
    {
        SPlugin *plugin = &registry->plugins[index];
        unload_func unload_function = (unload_func)plugin->unload_func;
        if(unload_function)
        {
            unload_function(registry, false);
            sp_internal_plugin_cleanup(plugin);
        }
    }
    free(registry->plugins);
    *registry = {};
}
//
//End API Registry ----------------------------------------------------



// Plugin Functions


SPlugin * sp_internal_win32_load_plugin(char* plugin_name, bool32 reloadable, APIRegistry *registry = nullptr)
{
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }
    SPlugin *plugin = sp_internal_api_registry_add_new_plugin(reg); 
    plugin->hash = SP_HASH(plugin_name);
    plugin->reloadable = reloadable;
        
    StrBuffer temp_plugin_name = {};
    sp_string_build_tmp_name(plugin_name, &temp_plugin_name);
    
    if(!CopyFile(plugin_name,temp_plugin_name.buffer,0))
    {
        return false;
        //@TODO: Log could not COPY plugin to temp_plugin.
    }

    plugin->library_handle = LoadLibraryA(temp_plugin_name.buffer);

    if(reloadable) 
    {
        //Get the information we need to monitor the plugin.
        plugin->file_handle = CreateFile(plugin_name,0,0,0,OPEN_EXISTING,0,0);
        FILETIME last_write_time = {};
        GetFileTime(plugin->file_handle,0,0,&last_write_time);
        plugin->last_write_time = last_write_time;
        //Add the plugin to the list so the registry can monitor it.
        reg->reloadable_plugins[reg->reloadable_count++] = plugin;
    }

    if(!plugin->library_handle)
    {
        return false;
        //@TODO: Log could not Load plugin
    }

    //call the plugin load function
    StrBuffer load_function_name = {};
    sp_string_build_load_function_name(plugin_name,&load_function_name);
    
    load_func load_function = (load_func)GetProcAddress(plugin->library_handle,load_function_name.buffer);
    if(!load_function)
    {
        SP_Assert(!"Could not locate the plugin load function!!!");
    }
    load_function(reg, false);

    //finally let's save the address of the unload function
    StrBuffer unload_function_name = {};
    sp_string_build_unload_function_name(plugin_name,&unload_function_name);
    plugin->unload_func = GetProcAddress(plugin->library_handle,unload_function_name.buffer);
    if(!plugin->unload_func)
    {
        SP_Assert(!"Could not locate the plugin UNLOAD function!!!");
    }
    
    return (plugin);
}


bool32 sp_internal_win32_reload_plugin(SPlugin* plugin, int32 index, APIRegistry* registry)
{
    //Load the new plugin
    char buffer[256];
    char plugin_name[256];
    StrBuffer new_plugin_name = {};
    GetFinalPathNameByHandle(plugin->file_handle, buffer, 256, VOLUME_NAME_NONE) ; 
    sp_string_extract_plugin_name(buffer, plugin_name);
    sp_string_build_tmp_name(plugin_name, &new_plugin_name, plugin->reload_count + 1);

    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    
    }

    SPlugin *new_plugin = sp_internal_api_registry_add_new_plugin(reg); 
    new_plugin->hash = SP_HASH(plugin_name);
    new_plugin->reloadable = 1;
    new_plugin->reload_count = plugin->reload_count + 1;
    new_plugin->file_handle = plugin->file_handle;
    new_plugin->last_write_time = plugin->last_write_time;
    
    if(!CopyFile(plugin_name,new_plugin_name.buffer,0))
    {
        return false;
        //@TODO: Log could not COPY plugin to temp_plugin.
    }

    new_plugin->library_handle = LoadLibraryA(new_plugin_name.buffer);
    //@TODO: Get the new time to keep track.

     if(!new_plugin->library_handle)
    {
        return false;
        //@TODO: Log could not Load plugin
    }
    StrBuffer load_function_name = {};
    sp_string_build_load_function_name(plugin_name,&load_function_name);
    
    load_func load_function = (load_func)GetProcAddress(new_plugin->library_handle,load_function_name.buffer);
    if(!load_function)
    {
        SP_Assert(!"Could not locate the plugin load function!!!");
    }
    load_function(reg, true);
    
    //finally let's save the address of the unload function
    StrBuffer unload_function_name = {};
    sp_string_build_unload_function_name(plugin_name,&unload_function_name);
    new_plugin->unload_func = GetProcAddress(new_plugin->library_handle,unload_function_name.buffer);
    if(!new_plugin->unload_func)
    {
        SP_Assert(!"Could not locate the plugin UNLOAD function!!!");
    }

    //@TODO: If I want to implement some sort of state transfer between the dlls it goes here.
    //       In here both plugins are loaded at the same time
    //
    //

    HMODULE old_plugin_handle = plugin->library_handle;

    unload_func unload_function = (unload_func)plugin->unload_func;
    unload_function(reg, true);

    FreeLibrary(old_plugin_handle);

    reg->reloadable_plugins[index] = new_plugin;


    
    return true;
}

SPlugin *
sp_load_plugin(APIRegistry *registry, char* plugin_name, bool32 reloadable)
{
    SPlugin *plugin = nullptr;
    #ifdef _WIN32
    plugin = sp_internal_win32_load_plugin(plugin_name, reloadable, registry);
    #else
        //@TODO: Add other OS
        #error NO OTHER OS DEFINED
    #endif //_WIN32

    return (plugin);
}
    

SPlugin *
sp_load_plugin(char* plugin_name, bool32 reloadable)
{
    return(sp_load_plugin(nullptr, plugin_name, reloadable));
}

void sp_unload_plugin(APIRegistry * registry,char* api_name)
{
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }

    uint64 desired_api_hash = SP_HASH(api_name);

    SPlugin *plugin = reg->plugins;
    uint32 count   = reg->capacity; 
    for(uint32 index = 0; index < count; ++index )
    {
        plugin = &reg->plugins[index];
        if(desired_api_hash == plugin->api_hash)
        {
            unload_func unload_function = (unload_func)plugin->unload_func;
            unload_function(reg, false);
            sp_internal_plugin_cleanup(plugin);
            *plugin = {}; //reset this slot
            break;
        }
    }
}

void sp_unload_plugin(APIRegistry * registry,SPlugin *plugin)
{
    APIRegistry *reg = registry;
    if(!reg)
    {
        reg = sp_internal_registry_get();
    }
    unload_func unload_function = (unload_func)plugin->unload_func;
    unload_function(reg, false);
    sp_internal_plugin_cleanup(plugin);
    *plugin = {}; //reset this slot
}

void sp_unload_plugin(char* api_name)
{
    sp_unload_plugin(nullptr, api_name);
}

void sp_unload_plugin(SPlugin *plugin)
{
    sp_unload_plugin(nullptr, plugin);
}




//End Plugin Functions

#endif //SIMPLE_PLUGIN_IMPLEMENTATION


/*
The MIT License (MIT)

Copyright (c) 2018 Filipe Rasoilo www.rasoilo.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
