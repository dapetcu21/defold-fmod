#ifndef FMOD_BRIDGE_H
#define FMOD_BRIDGE_H

#include <dmsdk/sdk.h>

namespace FMODBridge {
    void init(lua_State* L);
    void finalize();
}

#endif