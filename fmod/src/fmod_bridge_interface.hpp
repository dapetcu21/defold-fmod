#ifndef FMOD_BRIDGE_INTERFACE_H
#define FMOD_BRIDGE_INTERFACE_H

#define DLIB_LOG_DOMAIN "fmod"
#include <dmsdk/sdk.h>

// Using C symbol naming convention to reduce the chance of
// C++ compilers mangling symbols differently

extern "C" {
    typedef unsigned int FMODBridge_HBuffer;
    int FMODBridge_dmBuffer_GetBytes(FMODBridge_HBuffer, void**, uint32_t*);
    void FMODBridge_dmScript_PushBuffer(lua_State* L, FMODBridge_HBuffer);
    FMODBridge_HBuffer FMODBridge_dmScript_CheckBuffer(lua_State* L, int);
}

static void (*FMODBridge_init)(lua_State* L) = NULL;
static void (*FMODBridge_update)() = NULL;
static void (*FMODBridge_finalize)() = NULL;

#endif
