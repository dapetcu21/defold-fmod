#include <fmod_errors.h>

#include "fmod_bridge.hpp"
using namespace FMODBridge;

FMOD::Studio::System* FMODBridge::system = NULL;

void FMODBridge::init(lua_State *L) {
    printf("Initializing FMOD with lua state %p\n", L);

    FMOD_RESULT res = FMOD::Studio::System::create(&system);
    if (res != FMOD_OK) {
        printf("FMOD Error: %s\n", FMOD_ErrorString(res));
        system = NULL;
    }
}

void FMODBridge::finalize() {
    printf("Finalizing FMOD\n");

    if (system) {
        FMOD_RESULT res = system->release();
        if (res != FMOD_OK) { printf("FMOD Error: %s\n", FMOD_ErrorString(res)); }
        system = NULL;
    }
}
