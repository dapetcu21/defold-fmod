#include "fmod_bridge.hpp"
#ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY

#include <dlfcn.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#ifdef __linux__
#include <unistd.h>
#endif

void* FMODBridge::dlHandleLL = NULL;
void* FMODBridge::dlHandleST = NULL;

bool FMODBridge::linkLibraries() {
    if (FMODBridge::dlHandleLL && FMODBridge::dlHandleST) {
        return true;
    }

    if (FMODBridge::dlHandleST) { dlclose(dlHandleST); }
    if (FMODBridge::dlHandleLL) { dlclose(dlHandleLL); }

    char *exePath = NULL;
    const char *libPath = ".";
    bool mustFreeLibPath = false;

    #ifdef __linux__
    char* env = secure_getenv("DEFOLD_FMOD_LIB_PATH");
    #else
    char* env = getenv("DEFOLD_FMOD_LIB_PATH");
    #endif

    if (env && env[0]) {
        libPath = env;
    } else {
        #ifdef __APPLE__
        uint32_t bufsize = 0;
        _NSGetExecutablePath(NULL, &bufsize);
        exePath = new char[bufsize];
        _NSGetExecutablePath(exePath, &bufsize);
        libPath = dirname(exePath);
        char* newPath = new char[strlen(libPath) + 1];
        strcpy(newPath, libPath);
        libPath = newPath;
        mustFreeLibPath = true;
        #endif

        #ifdef __linux__
        exePath = new char[PATH_MAX + 2];
        ssize_t ret = readlink("/proc/self/exe", exePath, PATH_MAX + 2);
        if (ret >= 0 && ret <= PATH_MAX + 1) {
          exePath[ret] = 0;
          char* newPath = new char[ret + 1];
          strcpy(newPath, exePath);
          libPath = dirname(newPath); // dirname() clobbers newPath
          char* finalPath = new char[ret + 1];
          strcpy(finalPath, libPath);
          libPath = finalPath;
          mustFreeLibPath = true;
          delete[] newPath;
        } else {
          exePath[0] = 0;
        }
        #endif

        // Detect if the game is running in the editor

        #if defined(__APPLE__)
        #define FMB_PLATFORM_BUILD "darwin"
        #define FMB_PLATFORM "osx"
        #elif defined(__linux__)
        #define FMB_PLATFORM_BUILD "linux"
        #define FMB_PLATFORM "linux"
        #endif

        #if defined(__x86_64__)
        #define FMB_ARCH "x86_64"
        #elif defined(__i386)
        #define FMB_ARCH "x86"
        #endif

        #if defined(FMB_PLATFORM) && defined(FMB_ARCH)
        #define FMB_EDITOR_SUFFIX "/build/" FMB_ARCH "-" FMB_PLATFORM_BUILD "/dmengine"

        static const size_t suffixLen = strlen(FMB_EDITOR_SUFFIX);
        size_t exePathLen = strlen(exePath);
        if (exePathLen >= suffixLen && 0 == strcmp(FMB_EDITOR_SUFFIX, exePath + exePathLen - suffixLen)) {
            printf("INFO:fmod: Running in the editor. Will attempt to load libraries from project\n");

            exePath = dirname(dirname(dirname(exePath)));
            const char* resPath = FMODBridge_dmConfigFile_GetString("project.bundle_resources", "");

            #ifdef __APPLE__
            #define FMB_LIB_PATH "/" FMB_ARCH "-" FMB_PLATFORM "/Contents/MacOS"
            #else
            #define FMB_LIB_PATH "/" FMB_ARCH "-" FMB_PLATFORM
            #endif

            char* newPath = new char[strlen(exePath) + strlen(resPath) + strlen(FMB_LIB_PATH) + 1];
            strcpy(newPath, exePath);
            strcat(newPath, resPath);
            strcat(newPath, FMB_LIB_PATH);
            if (mustFreeLibPath) { delete[] libPath; }
            libPath = newPath;
            mustFreeLibPath = true;
        }

        #endif
    }

    #ifdef __APPLE__
    #define LIBEXT "dylib"
    #else
    #define LIBEXT "so"
    #endif

    if (exePath) { delete[] exePath; }
    size_t maxPathLen = strlen(libPath) + 20;
    exePath = new char[maxPathLen + 1];

    strcpy(exePath, libPath);
    strncat(exePath, "/libfmod." LIBEXT, maxPathLen);
    FMODBridge::dlHandleLL = dlopen(exePath, RTLD_NOW | RTLD_GLOBAL);
    if (!FMODBridge::dlHandleLL) { printf("WARNING:fmod: %s\n", dlerror()); }

    strcpy(exePath, libPath);
    strncat(exePath, "/libfmodstudio." LIBEXT, maxPathLen);
    FMODBridge::dlHandleST = dlopen(exePath, RTLD_NOW | RTLD_GLOBAL);
    if (!FMODBridge::dlHandleST) { printf("WARNING:fmod: %s\n", dlerror()); }

    if (mustFreeLibPath) { delete[] libPath; }
    delete[] exePath;
    return (FMODBridge::dlHandleLL && FMODBridge::dlHandleST);
}

#endif
