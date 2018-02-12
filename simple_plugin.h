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
    uint32 reload_count;
    bool32 reloadable;
    
    union {
        HMODULE handle;
    };

};

//Hash Functions
//@TODO: Try out MurmurHash3

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
//End Hash Functions

inline  int32
sp_string_length(char* string)
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
    int32 length = sp_string_length(string);

    int32 size = length / sizeof(string[0]);
    return(size);
}



bool32 sp_win32_load_plugin(SPlugin *plugin, char* plugin_name)
{
    *plugin = {}; //reset
    plugin->hash = sp_internal_djb2_hash(plugin_name);
    
    char* suffix = "_tmp";
    size_t terminator_size = 1;
    size_t size = sp_string_size(plugin_name) + sp_string_size(suffix) + terminator_size; 

    char* temp_plugin_name = (char*)malloc(size); 
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

    return true;

}





//[INTERNAL]
//API registry -- idea taken from "http://ourmachinery.com/post/little-machines-working-together-part-1/"
struct APIRegistry
{
    int32 capacity;
    int32 used;

    SPlugin *plugins;

    void (*add)(char* plugin_name);
    void (*remove)(char* plugin_name);

    void* (*get)(char* api_name);

};

void sp_internal_api_registry_add(char* plugin_name)
{

}

void sp_internal_api_registry_remove(char* plugin_name )
{

}

void * sp_internal_api_registry_get(char* api_name)
{
    return(0);
}

//@NOTE: Set the initial size of the registry to be big enough to contain SP_INITIAL_NUMBER_OG_PLUGINS
#define SP_INITIAL_NUMBER_OF_PLUGINS 10

//@NOTE: The se growth factor to grow the registry memory once we reach capacity.
#define SP_REGISTRY_GROWTH_FACTOR 2



internal APIRegistry sp_internal_registry_create(int32 initial_size = SP_INITIAL_NUMBER_OF_PLUGINS)
{
    APIRegistry reg = {};
    reg.capacity = initial_size;
    reg.plugins  = (SPlugin*)malloc(sizeof(SPlugin)*reg.capacity); 
    reg.add      = sp_internal_api_registry_add;
    reg.remove   = sp_internal_api_registry_remove;
    reg.get      = sp_internal_api_registry_get;
    return(reg);
}

APIRegistry registry = sp_internal_registry_create();