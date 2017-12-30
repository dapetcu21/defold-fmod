#ifndef FMOD_BRIDGE_H
#define FMOD_BRIDGE_H

#if defined(DM_PLATFORM_OSX)
#define PLATFORM_SUPPORTED

#define DLIB_LOG_DOMAIN "fmod"
#include <dmsdk/sdk.h>
#include <fmod_studio.hpp>
#include <fmod.hpp>

namespace FMODBridge {
    void init(lua_State* L);
    void update();
    void finalize();

    extern FMOD::Studio::System* system;
    extern FMOD::System* lowLevelSystem;

    void registerClasses(lua_State *L);
    void registerEnums(lua_State *L);
}

#endif
#endif
