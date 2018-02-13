#pragma once


//Simple Plugin --------------

//=============================================================================
// Summary :
//
//
//
//=============================================================================
// Revision History :
//
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
// 
//
// After defining SIMPLE_PLUGIN_IMPLEMENTATION
//
// 
//===============================================================================
//
// API  : API reference can be found further down.
//===============================================================================  




#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h> //malloc, realloc
#endif //_WIN32

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

struct SPlugin
{
    uint64 hash;
    uint64 api_hash;
    uint32 reload_count;
    bool32 reloadable;

    void* api;
    
    union {
        HMODULE handle;
    };

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



//[INTERNAL]
//API registry -- idea taken from "http://ourmachinery.com/post/little-machines-working-together-part-1/"



struct APIRegistry
{
    int32 capacity;
    int32 used;

    SPlugin *plugins;

    //Some helpers to speed up lookup of plugins and "holes".
    SPlugin *curr;
    int32 next_hole_index;

    void (*add)(char* plugin_name, void* api);
    void (*remove)(char* plugin_name);
    void* (*get)(char* api_name);

};


void sp_internal_api_registry_add(char* api_name, void* api);
void sp_internal_api_registry_remove(char* plugin_name );
void * sp_internal_api_registry_get(char* api_name);


#define SP_REGISTER_API(reg,api) reg->add(#api,&api)

//@FIXME: There was a weird bug when using realloc for the second time when this was set to 1.
#define SP_REGISTRY_INITIAL_CAPACITY 10
#define SP_REGISTRY_GROWTH_FACTOR    2

internal APIRegistry sp_internal_registry_create()
{
    APIRegistry reg = {};
    reg.capacity        = SP_REGISTRY_INITIAL_CAPACITY;
    reg.used            = 0;
    reg.plugins         = (SPlugin*)malloc(sizeof(SPlugin) * reg.capacity);
    memset(reg.plugins,0,sizeof(SPlugin) * reg.capacity); //set all plugin values to zero.
    reg.curr            = reg.plugins;
    reg.next_hole_index = 0;
    reg.add             = sp_internal_api_registry_add;
    reg.remove          = sp_internal_api_registry_remove;
    reg.get             = sp_internal_api_registry_get;
    return(reg);
}

global_variable APIRegistry sp_registry = sp_internal_registry_create();

internal APIRegistry* 
sp_registry_get()
{
    return (&sp_registry);
}

void sp_internal_api_registry_add(char* api_name, void* api)
{
    APIRegistry *reg = sp_registry_get();
    SPlugin *plugin = nullptr;

    plugin = reg->curr;
    plugin->api_hash = SP_HASH(api_name);
    plugin->api = api;

    reg->used++;

    
    if(reg->used < reg->capacity) 
    {
        //@TODO: This seems not necessary....
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

void sp_internal_api_registry_remove(char* api_name)
{
    APIRegistry *reg = sp_registry_get();

    uint64 desired_api_hash = SP_HASH(api_name);
    SPlugin *plugin = reg->plugins;
    uint32 count   = reg->capacity; 
    for(uint32 index = 0; index < count; ++index )
    {
        plugin = &reg->plugins[index];
        if(desired_api_hash == plugin->api_hash)
        {
            *plugin = {}; //reset this slot
            reg->curr = &reg->plugins[index];
            break;
        }
    }
    reg->used--;
}

void * sp_internal_api_registry_get(char* api_name)
{
    APIRegistry *reg = sp_registry_get();

    uint64 desired_api_hash = SP_HASH(api_name);
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

//Used to add a new plugin to the registry, if there is enough space all it does is return a 
//pointer to the curr plugin. If there is not enough space then we will reallocate, copy the old memory
//and then return a pointer to the new curr.
SPlugin* sp_internal_api_registry_add_new_plugin()
{
    APIRegistry *reg = sp_registry_get();
    if(reg->used < reg->capacity)
    {
        return(reg->curr);
    }
    else
    {
        int32 old_capacity = reg->capacity;
        reg->capacity = old_capacity*SP_REGISTRY_GROWTH_FACTOR;

        if(!realloc(reg->plugins,sizeof(SPlugin) * reg->capacity))
        {
            SP_Assert(!"Could not reallocate block");
        }
        size_t new_chunk_byte_size = sizeof(SPlugin) * (reg->capacity - old_capacity);
        memset(reg->plugins + old_capacity, 0, new_chunk_byte_size);

        reg->curr = reg->plugins + reg->used;

        return(reg->curr);
    }
}
//
//End API Registry ----------------------------------------------------

// String utilities ----------------------------------------------------
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
sp_string_build_load_function_name(char* plugin_name, char* load_function_name)
{
    char* prefix = "load_";
    char* c = load_function_name;
    *c++ = prefix[0]; //'l'
    *c++ = prefix[1]; //'o'
    *c++ = prefix[2]; //'a'
    *c++ = prefix[3]; //'d'
    *c++ = prefix[4]; //'_'

    char* t = plugin_name;
    while(*t)
    {
        if(*t == '.')
        {
            *c = '\0';
            break;
        }
        *c++ = *t++;
    }
}

// End String Utilities

// Plugin Functions

typedef void (*load_func)(APIRegistry *);

bool32 sp_win32_load_plugin(char* plugin_name, bool32 reloadable)
{
    APIRegistry *reg = sp_registry_get();
    SPlugin *plugin = sp_internal_api_registry_add_new_plugin(); 
    plugin->hash = SP_HASH(plugin_name);
    plugin->reloadable = reloadable;
    
    char* suffix = "_tmp";
    size_t terminator_size = 1;
    size_t size = sp_string_size(plugin_name) + sp_string_size(suffix) + terminator_size; 

    char* temp_plugin_name = (char*)malloc(size); ////@TODO: Use an internal buffer for these.
    char* t = temp_plugin_name;

    char *c = plugin_name;
    while(*c)
    {
        if(*c == '.')
        {
            *t++ = suffix[0];
            *t++ = suffix[1];
            *t++ = suffix[2];
            *t++ = suffix[3];
        }
        *t++ = *c++;
    }
    *t = '\0';

    if(!CopyFile(plugin_name,temp_plugin_name,0))
    {
        return false;
        //@TODO: Log could not COPY plugin to temp_plugin.
    }

    plugin->handle = LoadLibraryA(temp_plugin_name);

    if(!plugin->handle)
    {
        return false;
        //@TODO: Log could not Load plugin
    }

    //call the plugin load function

    //check if the plugin_name has extension and if yes then strip it.
    int32 strin_size_no_extension = sp_string_size_strip_extension(plugin_name);
    char* prefix = "load_";
    terminator_size = 1;
    size = sp_string_size(prefix) + strin_size_no_extension + terminator_size; 

    char* load_function_name = (char*)malloc(size); //@TODO: Use an internal buffer for these.
    sp_string_build_load_function_name(plugin_name,load_function_name);
    
    load_func load_function = (load_func)GetProcAddress(plugin->handle,load_function_name);
    if(!load_function)
    {
        SP_Assert(!"Could not locate the plugin load function!!!");
    }
    load_function(&sp_registry);

    //@TODO: Delete these calls to free and use an internal buffer.
    //free(temp_plugin_name);
    //free(load_function_name);
    
    return true;
}

//End Plugin Functions



