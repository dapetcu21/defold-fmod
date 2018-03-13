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

#ifdef _MSC_VER
#define LOGI(fmt, ...) ((void)printf("INFO:fmod: " fmt "\n", __VA_ARGS__))
#define LOGE(fmt, ...) ((void)printf("ERROR:fmod: " fmt "\n", __VA_ARGS__))
#define LOGW(fmt, ...) ((void)printf("WARNING:fmod: " fmt "\n", __VA_ARGS__))
#else
#define LOGI(...) ((void)printf("INFO:fmod: " FIRST(__VA_ARGS__) "\n" REST(__VA_ARGS__)))
#define LOGE(...) ((void)printf("ERROR:fmod: " FIRST(__VA_ARGS__) "\n" REST(__VA_ARGS__)))
#define LOGW(...) ((void)printf("WARNING:fmod: " FIRST(__VA_ARGS__) "\n" REST(__VA_ARGS__)))
#endif

#define STRINGIFY(x) #x
#define RESOLVE(x) x
#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define FIRST(...) FIRST_HELPER(__VA_ARGS__, throwaway)
#define FIRST_HELPER(first, ...) first
#define REST(...) REST_HELPER(NUM(__VA_ARGS__), __VA_ARGS__)
#define REST_HELPER(qty, ...) REST_HELPER2(qty, __VA_ARGS__)
#define REST_HELPER2(qty, ...) REST_HELPER_##qty(__VA_ARGS__)
#define REST_HELPER_ONE(first)
#define REST_HELPER_TWOORMORE(first, ...) , __VA_ARGS__
#define NUM(...) \
    SELECT_10TH(__VA_ARGS__, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE,\
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, ONE, throwaway)
#define SELECT_10TH(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a10

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
    extern bool isLinked;
    bool linkLibraries();
    void cleanupLibraries();
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
#define getSymbolPrintError(fname) LOGE("GetProcAddress(\"%s\"): %lu", STRINGIFY(fname), GetLastError())
#else
#define getSymbol dlsym
#define getSymbolPrintError(fname) LOGE("dlsym(\"%s\"): %s", STRINGIFY(fname), dlerror())
#endif
#define ensure(lib, fname, retType, ...) \
    static retType (F_CALL *fname)(__VA_ARGS__) = NULL; \
    if (!fname) { \
        fname = (retType (F_CALL *)(__VA_ARGS__))getSymbol(RESOLVE(CONCAT(FMODBridge::dlHandle, lib)), STRINGIFY(fname)); \
        if (!fname) { \
            getSymbolPrintError(fname); \
            abort(); \
        } \
    }
#else
#define ensure(lib, fname, retType, ...)
#endif

#endif
