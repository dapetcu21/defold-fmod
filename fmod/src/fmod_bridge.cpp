#include "fmod_bridge.hpp"

void FMODBridge::init(lua_State *L) {
    printf("Initializing FMOD with lua state %p\n", L);
}

void FMODBridge::finalize() {
    printf("Finalizing FMOD");
}
