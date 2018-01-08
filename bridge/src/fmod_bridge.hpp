#ifndef FMOD_BRIDGE_H
#define FMOD_BRIDGE_H

extern "C" {
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>
}

#include <stdint.h>
#include <fmod_studio.hpp>
#include <fmod.hpp>

#if defined(__APPLE__) || defined(__linux__)
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#endif

extern "C" {
    typedef unsigned int FMODBridge_HBuffer;
    int FMODBridge_dmBuffer_GetBytes(FMODBridge_HBuffer, void**, uint32_t*);
    void FMODBridge_dmScript_PushBuffer(lua_State* L, FMODBridge_HBuffer);
    FMODBridge_HBuffer FMODBridge_dmScript_CheckBuffer(lua_State* L, int);

    EXPORT void FMODBridge_init(lua_State* L);
    EXPORT void FMODBridge_update();
    EXPORT void FMODBridge_finalize();
}

namespace FMODBridge {
    extern FMOD::Studio::System* system;
    extern FMOD::System* lowLevelSystem;

    struct LuaHBuffer {
        FMODBridge_HBuffer handle;
    };

    void registerClasses(lua_State *L);
    void registerEnums(lua_State *L);
}

#endif
