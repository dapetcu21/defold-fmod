#ifndef FMOD_BRIDGE_H
#define FMOD_BRIDGE_H

extern "C" {
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>
}

#include <stdint.h>
#include <fmod_studio.h>
#include <fmod.h>

#if defined(__APPLE__) || defined(__linux__)
#define FMOD_BRIDGE_LOAD_DYNAMICALLY
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#endif

#define STRINGIFY(x) #x
#define RESOLVE(x) x
#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

extern "C" {
    typedef unsigned int FMODBridge_HBuffer;
    int FMODBridge_dmBuffer_GetBytes(FMODBridge_HBuffer, void**, uint32_t*);
    void FMODBridge_dmScript_PushBuffer(lua_State* L, FMODBridge_HBuffer);
    FMODBridge_HBuffer FMODBridge_dmScript_CheckBuffer(lua_State* L, int);

    void FMODBridge_init(lua_State* L);
    void FMODBridge_update();
    void FMODBridge_finalize();
}

namespace FMODBridge {
    extern FMOD_STUDIO_SYSTEM* system;
    extern FMOD_SYSTEM* lowLevelSystem;

    #ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
    extern void* dlHandleLL;
    extern void* dlHandleST;
    bool linkLibraries();
    #endif

    struct LuaHBuffer {
        FMODBridge_HBuffer handle;
    };

    void registerClasses(lua_State *L);
    void registerEnums(lua_State *L);
}

#ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
#define ensure(lib, fname, retType, ...) \
    static retType (*fname)(__VA_ARGS__) = NULL; \
    if (!fname) { \
        fname = (retType (*)(__VA_ARGS__))dlsym(RESOLVE(CONCAT(FMODBridge::dlHandle, lib)), STRINGIFY(fname)); \
        if (!fname) { \
            printf("ERROR:fmod: dlsym(\"%s\"): %s\n", STRINGIFY(fname), dlerror()); \
            abort(); \
        } \
    }
#else
#define ensure(lib, fname, retType, ...)
#endif

#endif
