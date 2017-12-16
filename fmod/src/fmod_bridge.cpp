#include <fmod_errors.h>
#include <dmsdk/sdk.h>
#include <LuaBridge/LuaBridge.h>

#include "fmod_bridge.hpp"

using namespace FMODBridge;
using namespace luabridge;

FMOD::Studio::System* FMODBridge::system = NULL;

namespace FMODBridge {
    void registerClasses(lua_State *L);
}

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

void FMODBridge::init(lua_State *L) {
    FMOD_RESULT res;

    res = FMOD::Studio::System::create(&system);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        system = NULL;
        return;
    }

    registerClasses(L);

    getGlobalNamespace(L)
        .beginNamespace("fmod")
            .beginNamespace("studio")
                .addVariable("system", &system, false)
            .endNamespace()
        .endNamespace();

    FMOD::System* lowLevelSystem;
    res = system->getLowLevelSystem(&lowLevelSystem);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
    } else {
        getGlobalNamespace(L)
            .beginNamespace("fmod")
                .addVariable("system", &lowLevelSystem, false)
            .endNamespace();
    }
}

void FMODBridge::finalize() {
    if (system) {
        FMOD_RESULT res = system->release();
        if (res != FMOD_OK) { printf("FMOD Error: %s\n", FMOD_ErrorString(res)); }
        system = NULL;
    }
}
