#define LIB_NAME "DefoldFMOD"
#define MODULE_NAME "fmod"

#ifndef DLIB_LOG_DOMAIN
#define DLIB_LOG_DOMAIN "fmod"
#endif
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_ANDROID)

#include "fmod_bridge_interface.hpp"

static dmConfigFile::HConfig appConfig;

dmExtension::Result AppInitializeDefoldFMOD(dmExtension::AppParams* params) {
    appConfig = params->m_ConfigFile;
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeDefoldFMOD(dmExtension::Params* params) {
    FMODBridge_init(params->m_L);
    return dmExtension::RESULT_OK;
}

dmExtension::Result UpdateDefoldFMOD(dmExtension::Params* params) {
    FMODBridge_update();
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeDefoldFMOD(dmExtension::Params* params) {
    FMODBridge_finalize();
    return dmExtension::RESULT_OK;
}

#if defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_ANDROID)
void OnEventDefoldFMOD(dmExtension::Params* params, const dmExtension::Event* event) {
    switch (event->m_Event) {
        case dmExtension::EVENT_ID_ACTIVATEAPP:
            FMODBridge_activateApp();
            break;
        case dmExtension::EVENT_ID_DEACTIVATEAPP:
            FMODBridge_deactivateApp();
            break;
    }
}
#else
#define OnEventDefoldFMOD 0
#endif

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

void FMODBridge_dmScript_PushVector3(lua_State* L, float x, float y, float z) {
    dmScript::PushVector3(L, Vectormath::Aos::Vector3(x, y, z));
}

FMODBridge_Vector3 FMODBridge_dmScript_CheckVector3(lua_State* L, int index) {
    Vectormath::Aos::Vector3 * vec = dmScript::CheckVector3(L, index);
    FMODBridge_Vector3 result = {
        .x = vec->getX(),
        .y = vec->getY(),
        .z = vec->getZ(),
    };
    return result;
}

const char* FMODBridge_dmConfigFile_GetString(const char* config, const char* defaultValue) {
    return dmConfigFile::GetString(appConfig, config, defaultValue);
}

int32_t FMODBridge_dmConfigFile_GetInt(const char* config, int32_t defaultValue) {
    return dmConfigFile::GetInt(appConfig, config, defaultValue);
}

#ifdef DM_PLATFORM_ANDROID
JavaVM* FMODBridge_dmGraphics_GetNativeAndroidJavaVM() {
    return dmGraphics::GetNativeAndroidJavaVM();
}

jobject FMODBridge_dmGraphics_GetNativeAndroidActivity() {
    return dmGraphics::GetNativeAndroidActivity();
}
#endif

#else

dmExtension::Result InitializeDefoldFMOD(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeDefoldFMOD(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

#define UpdateDefoldFMOD 0
#define AppInitializeDefoldFMOD 0
#define OnEventDefoldFMOD 0

#endif

DM_DECLARE_EXTENSION(DefoldFMOD, LIB_NAME, AppInitializeDefoldFMOD, 0, InitializeDefoldFMOD, UpdateDefoldFMOD, OnEventDefoldFMOD, FinalizeDefoldFMOD)
