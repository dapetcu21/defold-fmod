#include "fmod_bridge.hpp"
#include <fmod_errors.h>
#include <LuaBridge/LuaBridge.h>

using namespace FMODBridge;
using namespace luabridge;

FMOD::Studio::System* FMODBridge::system = NULL;
FMOD::System* FMODBridge::lowLevelSystem = NULL;

extern "C" EXPORT void FMODBridge_init(lua_State *L) {
    FMOD_RESULT res;

    res = FMOD::Studio::System::create(&FMODBridge::system);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        FMODBridge::system = NULL;
        return;
    }

    res = FMODBridge::system->getLowLevelSystem(&lowLevelSystem);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        FMODBridge::system->release();
        FMODBridge::system = NULL;
        return;
    }

    // TODO: Make this configurable somehow
    res = lowLevelSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        FMODBridge::system->release();
        FMODBridge::system = NULL;
        return;
    }

    void* extraDriverData = NULL;
    res = FMODBridge::system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        FMODBridge::system->release();
        FMODBridge::system = NULL;
        return;
    }

    registerEnums(L);
    registerClasses(L);
}

extern "C" EXPORT void FMODBridge_update() {
    if (FMODBridge::system) {
        FMOD_RESULT res = FMODBridge::system->update();
        if (res != FMOD_OK) {
            printf("FMOD Error: %s\n", FMOD_ErrorString(res));
            FMODBridge::system->release();
            FMODBridge::system = NULL;
            return;
        }
    }
}

extern "C" EXPORT void FMODBridge_finalize() {
    if (FMODBridge::system) {
        FMOD_RESULT res = FMODBridge::system->release();
        if (res != FMOD_OK) { printf("FMOD Error: %s\n", FMOD_ErrorString(res)); }
        FMODBridge::system = NULL;
    }
}
