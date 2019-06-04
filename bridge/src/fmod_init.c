#include "fmod_bridge.h"
#include <fmod_errors.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

FMOD_STUDIO_SYSTEM* FMODBridge_system = NULL;
FMOD_SYSTEM* FMODBridge_lowLevelSystem = NULL;
bool FMODBridge_isPaused = false;

#ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
bool FMODBridge_isLinked = false;
#endif

static FMOD_SPEAKERMODE speakerModeFromString(const char* str) {
    if (0 == strcmp(str, "default")) { return FMOD_SPEAKERMODE_DEFAULT; }
    if (0 == strcmp(str, "stereo")) { return FMOD_SPEAKERMODE_STEREO; }
    if (0 == strcmp(str, "mono")) { return FMOD_SPEAKERMODE_MONO; }
    if (0 == strcmp(str, "5.1")) { return FMOD_SPEAKERMODE_5POINT1; }
    if (0 == strcmp(str, "7.1")) { return FMOD_SPEAKERMODE_7POINT1; }
    if (0 == strcmp(str, "quad")) { return FMOD_SPEAKERMODE_QUAD; }
    if (0 == strcmp(str, "surround")) { return FMOD_SPEAKERMODE_SURROUND; }
    if (0 == strcmp(str, "max")) { return FMOD_SPEAKERMODE_MAX; }
    if (0 == strcmp(str, "raw")) { return FMOD_SPEAKERMODE_RAW; }
    LOGW("Invalid value for speaker_mode: \"%s\". Using default", str);
    return FMOD_SPEAKERMODE_DEFAULT;
}

#define check(fcall) do { \
    FMOD_RESULT res = fcall; \
    if (res != FMOD_OK) { \
        LOGE("%s", FMOD_ErrorString(res)); \
        FMOD_Studio_System_Release(FMODBridge_system); \
        FMODBridge_system = NULL; \
        return; \
    } \
} while(0)

void FMODBridge_init(lua_State *L) {
    #ifdef __EMSCRIPTEN__
    EM_ASM(Module.cwrap = Module.cwrap || cwrap);
    #endif

    #ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
    FMODBridge_isLinked = FMODBridge_linkLibraries();
    if (!FMODBridge_isLinked) {
        LOGW("FMOD libraries could not be loaded. FMOD will be disabled for this session");
        return;
    }
    #endif

    ensure(ST, FMOD_Studio_System_Create, FMOD_RESULT, FMOD_STUDIO_SYSTEM**, unsigned int);
    ensure(ST, FMOD_Studio_System_GetCoreSystem, FMOD_RESULT, FMOD_STUDIO_SYSTEM*, FMOD_SYSTEM**);
    ensure(LL, FMOD_System_SetSoftwareFormat, FMOD_RESULT, FMOD_SYSTEM*, int, FMOD_SPEAKERMODE, int);
    ensure(LL, FMOD_System_SetDSPBufferSize, FMOD_RESULT, FMOD_SYSTEM*, unsigned int, int);
    ensure(ST, FMOD_Studio_System_Initialize, FMOD_RESULT, FMOD_STUDIO_SYSTEM*, int, FMOD_STUDIO_INITFLAGS, FMOD_INITFLAGS, void*);
    ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);

    FMOD_RESULT res;
    res = FMOD_Studio_System_Create(&FMODBridge_system, FMOD_VERSION);
    if (res != FMOD_OK) {
        LOGE("%s", FMOD_ErrorString(res));
        FMODBridge_system = NULL;
        return;
    }

    check(FMOD_Studio_System_GetCoreSystem(FMODBridge_system, &FMODBridge_lowLevelSystem));

    int defaultSampleRate = 0;
    #ifdef __EMSCRIPTEN__
    check(FMOD_System_GetDriverInfo(FMODBridge_lowLevelSystem, 0, NULL, 0, NULL, &defaultSampleRate, NULL, NULL));
    check(FMOD_System_SetDSPBufferSize(FMODBridge_lowLevelSystem, 2048, 2));
    #endif

    int sampleRate = FMODBridge_dmConfigFile_GetInt("fmod.sample_rate", defaultSampleRate);
    int numRawSpeakers = FMODBridge_dmConfigFile_GetInt("fmod.num_raw_speakers", 0);
    const char* speakerModeStr = FMODBridge_dmConfigFile_GetString("fmod.speaker_mode", "default");
    FMOD_SPEAKERMODE speakerMode = speakerModeFromString(speakerModeStr);

    if (sampleRate || numRawSpeakers || speakerMode != FMOD_SPEAKERMODE_DEFAULT) {
        check(FMOD_System_SetSoftwareFormat(FMODBridge_lowLevelSystem, sampleRate, speakerMode, numRawSpeakers));
    }

    FMOD_STUDIO_INITFLAGS studioInitFlags = FMOD_STUDIO_INIT_NORMAL;
    if (FMODBridge_dmConfigFile_GetInt("fmod.live_update", 0)) {
        studioInitFlags |= FMOD_STUDIO_INIT_LIVEUPDATE;
    }

    void* extraDriverData = NULL;
    check(FMOD_Studio_System_Initialize(FMODBridge_system, 1024, studioInitFlags, FMOD_INIT_NORMAL, extraDriverData));

    FMODBridge_isPaused = false;

    #ifdef __EMSCRIPTEN__
    EM_ASM({
        Module._FMODBridge_onClick = function () {
            ccall('FMODBridge_unmuteAfterUserInteraction', null, [], []);
            Module.canvas.removeEventListener('click', Module._FMODBridge_onClick);
        };
        Module.canvas.addEventListener('click', Module._FMODBridge_onClick, false);
    });
    #endif

    #if TARGET_OS_IPHONE
    FMODBridge_initIOSInterruptionHandler();
    #endif

    FMODBridge_register(L);
}

void FMODBridge_update() {
    if (!FMODBridge_system || FMODBridge_isPaused) { return; }

    ensure(ST, FMOD_Studio_System_Update, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);
    ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);

    FMOD_RESULT res = FMOD_Studio_System_Update(FMODBridge_system);
    if (res != FMOD_OK) {
        LOGE("%s", FMOD_ErrorString(res));
        FMOD_Studio_System_Release(FMODBridge_system);
        FMODBridge_system = NULL;
        return;
    }
}

void FMODBridge_finalize() {
    if (FMODBridge_system) {
        ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);

        FMOD_RESULT res = FMOD_Studio_System_Release(FMODBridge_system);
        if (res != FMOD_OK) { LOGE("%s", FMOD_ErrorString(res)); }
        FMODBridge_system = NULL;
    }

    #ifdef __EMSCRIPTEN__
    EM_ASM({
        Module.canvas.removeEventListener('click', Module._FMODBridge_onClick);
    });
    #endif

    #ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY
    if (FMODBridge_isLinked) { FMODBridge_cleanupLibraries(); }
    #endif
}

void FMODBridge_resumeMixer() {
    if (FMODBridge_system && FMODBridge_isPaused) {
        ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);
        ensure(LL, FMOD_System_MixerResume, FMOD_RESULT, FMOD_SYSTEM*);
        check(FMOD_System_MixerResume(FMODBridge_lowLevelSystem));
        FMODBridge_isPaused = false;
    }
}

void FMODBridge_suspendMixer() {
    if (FMODBridge_system && !FMODBridge_isPaused) {
        ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);
        ensure(LL, FMOD_System_MixerSuspend, FMOD_RESULT, FMOD_SYSTEM*);
        check(FMOD_System_MixerSuspend(FMODBridge_lowLevelSystem));
        FMODBridge_isPaused = true;
    }
}

#ifdef __EMSCRIPTEN__
__attribute__((used))
void FMODBridge_unmuteAfterUserInteraction() {
    if (FMODBridge_system && !FMODBridge_isPaused) {
        ensure(ST, FMOD_Studio_System_Release, FMOD_RESULT, FMOD_STUDIO_SYSTEM*);
        ensure(LL, FMOD_System_MixerSuspend, FMOD_RESULT, FMOD_SYSTEM*);
        ensure(LL, FMOD_System_MixerResume, FMOD_RESULT, FMOD_SYSTEM*);
        check(FMOD_System_MixerSuspend(FMODBridge_lowLevelSystem));
        check(FMOD_System_MixerResume(FMODBridge_lowLevelSystem));
    }
}
#endif

void FMODBridge_activateApp() {
    #if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
    FMODBridge_resumeMixer();
    #endif
}

void FMODBridge_deactivateApp() {
    #if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
    FMODBridge_suspendMixer();
    #endif
}
