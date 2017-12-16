// myextension.cpp
// Extension lib defines
#define LIB_NAME "DefoldFMOD"
#define MODULE_NAME "fmod"

// include the Defold SDK
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_OSX)

dmExtension::Result InitializeDefoldFMOD(dmExtension::Params* params)
{
    lua_State* L = params->m_L;
    printf("Initializing FMOD with lua state %p\n", L);
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeDefoldFMOD(dmExtension::Params* params)
{
    printf("Finalizing FMOD\n");
    return dmExtension::RESULT_OK;
}

#else

dmExtension::Result InitializeDefoldFMOD(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeDefoldFMOD(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

#endif

dmExtension::Result AppInitializeDefoldFMOD(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeDefoldFMOD(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(DefoldFMOD, LIB_NAME, AppInitializeDefoldFMOD, AppFinalizeDefoldFMOD, InitializeDefoldFMOD, 0, 0, FinalizeDefoldFMOD)