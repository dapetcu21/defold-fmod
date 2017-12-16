#ifndef FMOD_BRIDGE_H
#define FMOD_BRIDGE_H

#include <dmsdk/sdk.h>
#include <fmod_studio.hpp>
#include <fmod.hpp>

namespace FMODBridge {
    void init(lua_State* L);
    void finalize();

    extern FMOD::Studio::System* system;
}

#endif