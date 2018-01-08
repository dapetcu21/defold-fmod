#include "fmod_bridge.hpp"
#include <fmod_errors.h>
#include <LuaBridge/LuaBridge.h>

using namespace FMODBridge;
using namespace luabridge;

FMOD_STUDIO_SYSTEM* FMODBridge::system = NULL;
FMOD_SYSTEM* FMODBridge::lowLevelSystem = NULL;

extern "C" EXPORT void FMODBridge_init(lua_State *L) {
    FMOD_RESULT res;

    res = FMOD_Studio_System_Create(&FMODBridge::system, FMOD_VERSION);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        FMODBridge::system = NULL;
        return;
    }

    res = FMOD_Studio_System_GetLowLevelSystem(FMODBridge::system, &lowLevelSystem);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        FMOD_Studio_System_Release(FMODBridge::system);
        FMODBridge::system = NULL;
        return;
    }

    // TODO: Make this configurable somehow
    res = FMOD_System_SetSoftwareFormat(lowLevelSystem, 0, FMOD_SPEAKERMODE_5POINT1, 0);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        FMOD_Studio_System_Release(FMODBridge::system);
        FMODBridge::system = NULL;
        return;
    }

    void* extraDriverData = NULL;
    res = FMOD_Studio_System_Initialize(FMODBridge::system, 1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        FMOD_Studio_System_Release(FMODBridge::system);
        FMODBridge::system = NULL;
        return;
    }

    registerEnums(L);
    registerClasses(L);
}

extern "C" EXPORT void FMODBridge_update() {
    if (FMODBridge::system) {
        FMOD_RESULT res = FMOD_Studio_System_Update(FMODBridge::system);
        if (res != FMOD_OK) {
            printf("FMOD Error: %s\n", FMOD_ErrorString(res));
            FMOD_Studio_System_Release(FMODBridge::system);
            FMODBridge::system = NULL;
            return;
        }
    }
}

extern "C" EXPORT void FMODBridge_finalize() {
    if (FMODBridge::system) {
        FMOD_RESULT res = FMOD_Studio_System_Release(FMODBridge::system);
        if (res != FMOD_OK) { printf("FMOD Error: %s\n", FMOD_ErrorString(res)); }
        FMODBridge::system = NULL;
    }
}
