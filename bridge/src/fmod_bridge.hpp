#ifndef FMOD_BRIDGE_H
#define FMOD_BRIDGE_H

extern "C" {
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>
}

#include <stdint.h>
#include <fmod_studio.h>
#include <fmod.h>

#ifdef __APPLE__
    #include "TargetConditionals.h"
#endif

#ifdef _WIN32
    #include <Windows.h>
#endif

#if !defined(FMOD_FORCE_STATIC_LINK) && ((defined(__APPLE__) && !TARGET_OS_IPHONE) || defined(__linux__) || defined(_WIN32))
    #define FMOD_BRIDGE_LOAD_DYNAMICALLY
    #include <stdlib.h>
    #include <stdio.h>
    #ifndef _WIN32
        #include <dlfcn.h>
        #define dlModuleT void *
    #else
        #define dlModuleT HMODULE
    #endif
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
    const char* FMODBridge_dmConfigFile_GetString(const char*, const char*);
    int32_t FMODBridge_dmConfigFile_GetInt(const char*, int32_t);

    void FMODBridge_init(lua_State* L);
    void FMODBridge_update();
    void FMODBridge_finalize();
    void FMODBridge_activateApp();
    void FMODBridge_deactivateApp();
}

namespace FMODBridge {
    extern FMOD_STUDIO_SYSTEM* system;
    extern FMOD_SYSTEM* lowLevelSystem;
    extern bool isPaused;

    #ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
    extern dlModuleT dlHandleLL;
    extern dlModuleT dlHandleST;
    bool linkLibraries();
    #endif

    struct LuaHBuffer {
        FMODBridge_HBuffer handle;
    };

    void registerClasses(lua_State *L);
    void registerEnums(lua_State *L);
}

#ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
#ifdef _WIN32
#define getSymbol GetProcAddress
#define getSymbolPrintError() printf("ERROR:fmod: GetProcAddress(\"%s\"): %lu\n", STRINGIFY(fname), GetLastError())
#else
#define getSymbol dlsym
#define getSymbolPrintError() printf("ERROR:fmod: dlsym(\"%s\"): %s\n", STRINGIFY(fname), dlerror())
#endif
#define ensure(lib, fname, retType, ...) \
    static retType (F_CALL *fname)(__VA_ARGS__) = NULL; \
    if (!fname) { \
        fname = (retType (F_CALL *)(__VA_ARGS__))getSymbol(RESOLVE(CONCAT(FMODBridge::dlHandle, lib)), STRINGIFY(fname)); \
        if (!fname) { \
            getSymbolPrintError(); \
            abort(); \
        } \
    }
#else
#define ensure(lib, fname, retType, ...)
#endif

#endif
