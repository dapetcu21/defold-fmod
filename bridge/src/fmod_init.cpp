#include "fmod_bridge.hpp"
#include <fmod_errors.h>
#include <LuaBridge/LuaBridge.h>

using namespace FMODBridge;
using namespace luabridge;

FMOD_STUDIO_SYSTEM* FMODBridge::system = NULL;
FMOD_SYSTEM* FMODBridge::lowLevelSystem = NULL;

extern "C" void FMODBridge_init(lua_State *L) {
    #ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
    if (!linkLibraries()) {
        printf("WARNING:fmod: FMOD libraries could not be loaded. FMOD will be disabled for this session\n");
        return;
    }
    #endif

    ensure(ST, FMOD_Studio_System_Create, FMOD_RESULT, FMOD_STUDIO_SYSTEM**, unsigned int);
    ensure(ST, FMOD_Studio_System_GetLowLevelSystem, FMOD_RESULT, FMOD_STUDIO_SYSTEM*, FMOD_SYSTEM**);
    ensure(LL, FMOD_System_SetSoftwareFormat, FMOD_RESULT, FMOD_SYSTEM*, int, FMOD_SPEAKERMODE, int);
    ensure(ST, FMOD_Studio_System_Initialize, FMOD_RESULT, FMOD_STUDIO_SYSTEM*, int, FMOD_STUDIO_INITFLAGS, FMOD_INITFLAGS, void*);
    ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);

    FMOD_RESULT res;

    res = FMOD_Studio_System_Create(&FMODBridge::system, FMOD_VERSION);
    if (res != FMOD_OK) {
        printf("ERROR:fmod: %s\n", FMOD_ErrorString(res));
        FMODBridge::system = NULL;
        return;
    }

    res = FMOD_Studio_System_GetLowLevelSystem(FMODBridge::system, &lowLevelSystem);
    if (res != FMOD_OK) {
        printf("ERROR:fmod: %s\n", FMOD_ErrorString(res));
        FMOD_Studio_System_Release(FMODBridge::system);
        FMODBridge::system = NULL;
        return;
    }

    // TODO: Make this configurable somehow
    res = FMOD_System_SetSoftwareFormat(lowLevelSystem, 0, FMOD_SPEAKERMODE_5POINT1, 0);
    if (res != FMOD_OK) {
        printf("ERROR:fmod: %s\n", FMOD_ErrorString(res));
        FMOD_Studio_System_Release(FMODBridge::system);
        FMODBridge::system = NULL;
        return;
    }

    void* extraDriverData = NULL;
    res = FMOD_Studio_System_Initialize(FMODBridge::system, 1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData);
    if (res != FMOD_OK) {
        printf("ERROR:fmod: %s\n", FMOD_ErrorString(res));
        FMOD_Studio_System_Release(FMODBridge::system);
        FMODBridge::system = NULL;
        return;
    }

    registerEnums(L);
    registerClasses(L);
}

extern "C" void FMODBridge_update() {
    #ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
    if (!FMODBridge::dlHandleLL || !FMODBridge::dlHandleST) { return; }
    #endif

    ensure(ST, FMOD_Studio_System_Update, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);
    ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);

    if (FMODBridge::system) {
        FMOD_RESULT res = FMOD_Studio_System_Update(FMODBridge::system);
        if (res != FMOD_OK) {
            printf("ERROR:fmod: %s\n", FMOD_ErrorString(res));
            FMOD_Studio_System_Release(FMODBridge::system);
            FMODBridge::system = NULL;
            return;
        }
    }
}

extern "C" void FMODBridge_finalize() {
    #ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
    if (!FMODBridge::dlHandleLL || !FMODBridge::dlHandleST) { return; }
    #endif

    ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);

    if (FMODBridge::system) {
        FMOD_RESULT res = FMOD_Studio_System_Release(FMODBridge::system);
        if (res != FMOD_OK) { printf("ERROR:fmod: %s\n", FMOD_ErrorString(res)); }
        FMODBridge::system = NULL;
    }
}
