#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <fmod_errors.h>

#include "fmod_bridge.hpp"

void FMODBridge::init(lua_State *L) {
    printf("Initializing FMOD with lua state %p\n", L);

    FMOD::Studio::System* system = NULL;
    FMOD_RESULT res = FMOD::Studio::System::create(&system);
    if (res != FMOD_OK) { printf("%s", FMOD_ErrorString(res)); }
}

void FMODBridge::finalize() {
    printf("Finalizing FMOD");
}
