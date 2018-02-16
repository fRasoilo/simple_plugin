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
//  - Support for differnt OS  
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

//===============================================================================  
// [IMPLEMENTATION]
//=============================================================================== 


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



//[INTERNAL]
//API registry -- idea taken from "http://ourmachinery.com/post/little-machines-working-together-part-1/"

#define SP_MAX_PLUGINS 100

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


//Typedefs of the plugin load and unload function signatures
typedef void (*load_func)(APIRegistry *, bool32);
typedef void (*unload_func)(APIRegistry *, bool32);


void sp_internal_api_registry_add(char* api_name, void* api, bool32 reload, APIRegistry *registry);
void sp_internal_api_registry_remove(char* plugin_name, bool32 reload, APIRegistry *registry);
void * sp_internal_api_registry_get(uint64 api_hash, APIRegistry *registry);
void sp_internal_api_registry_transfer_state(void* old_state, void* new_sate);

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

#define SP_REGISTRY_INITIAL_CAPACITY 10
#define SP_REGISTRY_GROWTH_FACTOR    2

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
                //@TODO: Win32 only.
                CloseHandle(plugin->file_handle);
                FreeLibrary(plugin->library_handle);

                *plugin = {}; //reset this slot
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

bool32 sp_internal_plugin_modified(SPlugin *plugin)
{
    //@TODO: This is Win32 specific for now:
    FILETIME last_write_time = {};
    GetFileTime(plugin->file_handle,0,0,&last_write_time);
    
    bool32 modified = CompareFileTime(&plugin->last_write_time,&last_write_time);
    if(modified)
    {
        plugin->last_write_time = last_write_time;
    }

    return(modified);
}

//Forward declare.
bool32 sp_internal_win32_reload_plugin(SPlugin* plugin, int32 index, APIRegistry *registry);


void sp_internal_api_registry_check_reloadable_plugins(APIRegistry *registry)
{
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
            
            //@TODO: Refactor this: WIN32 RELOAD PLUGIN
            sp_internal_win32_reload_plugin(plugin, index, reg);
            
            //
            //
        }
    }
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
    load_function(&sp_registry, false);

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
            break;
        }
    }
}

void sp_unload_plugin(APIRegistry * registry,SPlugin *plugin)
{
    unload_func unload_function = (unload_func)plugin->unload_func;
    unload_function(registry, false);
}

void sp_unload_plugin(char* api_name)
{
    sp_unload_plugin(nullptr, api_name);
}

void sp_unload_plugin(SPlugin *plugin)
{
    APIRegistry *reg = sp_internal_registry_get();
    unload_func unload_function = (unload_func)plugin->unload_func;
    unload_function(reg, false);
}


//End Plugin Functions



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
