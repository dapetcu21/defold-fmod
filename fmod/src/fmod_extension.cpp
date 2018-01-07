#define LIB_NAME "DefoldFMOD"
#define MODULE_NAME "fmod"

#if defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)

#if defined(DM_PLATFORM_OSX)
#include <dlfcn.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <string>
#endif

#include "fmod_bridge_interface.hpp"

#define LINK_DYNAMICALLY defined(DM_PLATFORM_OSX)

static void FMODBridge_link() {
#if LINK_DYNAMICALLY
    if (FMODBridge_init) { return; }

    uint32_t bufsize = 0;
    _NSGetExecutablePath(NULL, &bufsize);
    char* path = new char[bufsize];
    _NSGetExecutablePath(path, &bufsize);
    std::string libpath(dirname(path));
    delete[] path;

    void* fmod_handle = dlopen((libpath + "/libfmod.dylib").c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!fmod_handle) { dmLogWarning("%s", dlerror()); }

    void* fmodstudio_handle = dlopen((libpath + "/libfmodstudio.dylib").c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!fmodstudio_handle) { dmLogWarning("%s", dlerror()); }

    void* fmodbridge_handle = dlopen((libpath + "/libfmodbridge.dylib").c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!fmodbridge_handle) { dmLogWarning("%s", dlerror()); }

    FMODBridge_init = (void (*)(lua_State*))dlsym(fmodbridge_handle, "FMODBridge_init");
    FMODBridge_update = (void (*)())dlsym(fmodbridge_handle, "FMODBridge_update");
    FMODBridge_finalize = (void (*)())dlsym(fmodbridge_handle, "FMODBridge_finalize");
#endif
}

dmExtension::Result InitializeDefoldFMOD(dmExtension::Params* params) {
    FMODBridge_link();
#if LINK_DYNAMICALLY
    if (FMODBridge_init) {
        FMODBridge_init(params->m_L);
    }
#else
    FMODBridge_init(params->m_L);
#endif
    return dmExtension::RESULT_OK;
}

dmExtension::Result UpdateDefoldFMOD(dmExtension::Params* params) {
#if LINK_DYNAMICALLY
    if (FMODBridge_update) {
        FMODBridge_update();
    }
#else
    FMODBridge_update();
#endif
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeDefoldFMOD(dmExtension::Params* params) {
#if LINK_DYNAMICALLY
    if (FMODBridge_finalize) {
        FMODBridge_finalize();
    }
#else
    FMODBridge_finalize();
#endif
    return dmExtension::RESULT_OK;
}

int FMODBridge_dmBuffer_GetBytes(FMODBridge_HBuffer buffer, void** bytes, uint32_t* size) {
    return dmBuffer::GetBytes((dmBuffer::HBuffer)buffer, bytes, size) != dmBuffer::RESULT_OK;
}

void FMODBridge_dmScript_PushBuffer(lua_State* L, FMODBridge_HBuffer buffer) {
    dmScript::LuaHBuffer wrapper = { buffer, false };
    dmScript::PushBuffer(L, wrapper);
}

FMODBridge_HBuffer FMODBridge_dmScript_CheckBuffer(lua_State* L, int index) {
    return dmScript::CheckBuffer(L, index)->m_Buffer;
}

#else

dmExtension::Result InitializeDefoldFMOD(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

#define UpdateDefoldFMOD 0

dmExtension::Result FinalizeDefoldFMOD(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

#endif

dmExtension::Result AppInitializeDefoldFMOD(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeDefoldFMOD(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(DefoldFMOD, LIB_NAME, AppInitializeDefoldFMOD, AppFinalizeDefoldFMOD, InitializeDefoldFMOD, UpdateDefoldFMOD, 0, FinalizeDefoldFMOD)

// Stub extension so the user can link to 3rd party libs

dmExtension::Result InitializeDefoldFMODStub(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeDefoldFMODStub(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppInitializeDefoldFMODStub(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeDefoldFMODStub(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(DefoldFMODStub, "fmodstub", AppInitializeDefoldFMODStub, AppFinalizeDefoldFMODStub, InitializeDefoldFMODStub, 0, 0, FinalizeDefoldFMODStub)
