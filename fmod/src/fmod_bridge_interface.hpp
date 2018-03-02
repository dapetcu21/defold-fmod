#ifndef FMOD_BRIDGE_INTERFACE_H
#define FMOD_BRIDGE_INTERFACE_H

// Using C symbol naming convention to reduce the chance of
// C++ compilers mangling symbols differently

extern "C" {
    typedef unsigned int FMODBridge_HBuffer;
    int FMODBridge_dmBuffer_GetBytes(FMODBridge_HBuffer, void**, uint32_t*);
    void FMODBridge_dmScript_PushBuffer(lua_State* L, FMODBridge_HBuffer);
    FMODBridge_HBuffer FMODBridge_dmScript_CheckBuffer(lua_State* L, int);
    const char* FMODBridge_dmConfigFile_GetString(const char*, const char*);
    int32_t FMODBridge_dmConfigFile_GetInt(const char*, int32_t);
    #ifdef DM_PLATFORM_ANDROID
    JavaVM* FMODBridge_dmGraphics_GetNativeAndroidJavaVM();
    jobject FMODBridge_dmGraphics_GetNativeAndroidActivity();
    #endif

    void FMODBridge_init(lua_State* L);
    void FMODBridge_update();
    void FMODBridge_finalize();
    void FMODBridge_activateApp();
    void FMODBridge_deactivateApp();
}

#endif
