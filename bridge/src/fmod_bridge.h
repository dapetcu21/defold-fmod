#ifndef FMOD_BRIDGE_H
#define FMOD_BRIDGE_H

#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fmod_studio.h>
#include <fmod.h>

#ifdef __APPLE__
    #include "TargetConditionals.h"
#endif

#ifdef _WIN32
    #include <Windows.h>
#endif

#if !defined(FMOD_FORCE_STATIC_LINK) && ((defined(__APPLE__) && !TARGET_OS_IPHONE) || (defined(__linux__) && !defined(__ANDROID__)) || defined(__ANDROID__) || defined(_WIN32))
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

#ifdef __ANDROID__
#include <android/log.h>
#include <jni.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "fmod", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "fmod", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "fmod", __VA_ARGS__))
#elif defined(_MSC_VER)
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

// Extension API
typedef unsigned int FMODBridge_HBuffer;
int FMODBridge_dmBuffer_GetBytes(FMODBridge_HBuffer, void**, uint32_t*);
void FMODBridge_dmScript_PushBuffer(lua_State* L, FMODBridge_HBuffer);
FMODBridge_HBuffer FMODBridge_dmScript_CheckBuffer(lua_State* L, int);
void FMODBridge_dmScript_PushVector3(lua_State* L, float x, float y, float z);
FMOD_VECTOR FMODBridge_dmScript_CheckVector3(lua_State* L, int index);
const char* FMODBridge_dmConfigFile_GetString(const char*, const char*);
int32_t FMODBridge_dmConfigFile_GetInt(const char*, int32_t);

#ifdef __ANDROID__
JavaVM* FMODBridge_dmGraphics_GetNativeAndroidJavaVM();
jobject FMODBridge_dmGraphics_GetNativeAndroidActivity();
#endif

// Lifecycle functions
void FMODBridge_init(lua_State* L);
void FMODBridge_update();
void FMODBridge_finalize();
void FMODBridge_activateApp();
void FMODBridge_deactivateApp();

// Internal functions and members
extern FMOD_STUDIO_SYSTEM* FMODBridge_system;
extern FMOD_SYSTEM* FMODBridge_lowLevelSystem;
extern bool FMODBridge_isPaused;

#ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
extern dlModuleT FMODBridge_dlHandleLL;
extern dlModuleT FMODBridge_dlHandleST;
extern bool FMODBridge_isLinked;
bool FMODBridge_linkLibraries();
void FMODBridge_cleanupLibraries();
#endif

void FMODBridge_register(lua_State *L);
int FMODBridge_getBundleRoot(lua_State *L);

void FMODBridge_suspendMixer();
void FMODBridge_resumeMixer();

#if TARGET_OS_IPHONE
void FMODBridge_initIOSInterruptionHandler();
#endif

#ifdef __ANDROID__
void FMODBridge_attachJNI();
void FMODBridge_detachJNI();
#define attachJNI() FMODBridge_attachJNI()
#define detachJNI() FMODBridge_detachJNI()
#else
#define attachJNI()
#define detachJNI()
#endif

#ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY

#ifdef _WIN32
#define getSymbol GetProcAddress
#define getSymbolPrintError(fname) LOGE("GetProcAddress(\"%s\"): %lu", STRINGIFY(fname), GetLastError())
#else
#define getSymbol dlsym
#define getSymbolPrintError(fname) LOGE("dlsym(\"%s\"): %s", STRINGIFY(fname), dlerror())
#endif

#define ensure_(lib, fname, retType, ...) \
    static retType (F_CALL *fname)(__VA_ARGS__) = NULL; \
    if (!fname) { \
        fname = (retType (F_CALL *)(__VA_ARGS__))getSymbol(RESOLVE(CONCAT(FMODBridge_dlHandle, lib)), STRINGIFY(fname)); \
        if (!fname) { \
            getSymbolPrintError(fname); \
            abort(); \
        } \
    }

#define ensure(lib, fname, retType, ...) ensure_(lib, fname, retType, __VA_ARGS__)

#else
#define ensure(lib, fname, retType, ...)
#endif

#endif
