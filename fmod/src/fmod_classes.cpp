#include "fmod_bridge.hpp"
#include <LuaBridge/LuaBridge.h>

using namespace luabridge;

void FMODBridge::registerClasses(lua_State *L) {
    getGlobalNamespace(L)
        .beginNamespace("fmod")
            .beginClass<FMOD::System>("System")
            .endClass()
            .beginNamespace("studio")
                .beginClass<FMOD::Studio::System>("System")
                .endClass()
            .endNamespace()
        .endNamespace();
}
