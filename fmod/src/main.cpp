#define LIB_NAME "DefoldFMOD"
#define MODULE_NAME "fmod"

#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_OSX)

#include "fmod_bridge.hpp"

dmExtension::Result InitializeDefoldFMOD(dmExtension::Params* params)
{
    FMODBridge::init(params->m_L);
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeDefoldFMOD(dmExtension::Params* params)
{
    FMODBridge::finalize();
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