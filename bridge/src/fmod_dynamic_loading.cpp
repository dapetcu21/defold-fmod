#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "fmod_bridge.hpp"
#ifdef FMOD_BRIDGE_LOAD_DYNAMICALLY

#ifndef _WIN32
#include <dlfcn.h>
#include <libgen.h>
#else
#include <Shlwapi.h>
#endif

#include <string.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#ifdef __linux__
#include <unistd.h>
#endif

dlModuleT FMODBridge::dlHandleLL = NULL;
dlModuleT FMODBridge::dlHandleST = NULL;

#if defined(_WIN32)
    #define SEP "\\"
    #define SEPCH '\\'
#else
    #define SEP "/"
    #define SEPCH '/'
#endif

#ifdef _WIN32
static char * dirname(char * path) {
    size_t i = strlen(path);
    do {
        i -= 1;
        if (path[i] == SEPCH) {
            path[i] = 0;
            break;
        }
    } while (i);
    return path;
}
#endif

#ifdef __ANDROID__
JNIEnv* FMODBridge::attachJNI()
{
    JNIEnv* env;
    JavaVM* vm = FMODBridge_dmGraphics_GetNativeAndroidJavaVM();
    vm->AttachCurrentThread(&env, NULL);
    return env;
}

bool FMODBridge::detachJNI(JNIEnv* env)
{
    bool exception = (bool) env->ExceptionCheck();
    env->ExceptionClear();
    JavaVM* vm = FMODBridge_dmGraphics_GetNativeAndroidJavaVM();
    vm->DetachCurrentThread();
    return !exception;
}

jclass FMODBridge::jniGetClass(JNIEnv* env, const char* classname) {
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jmethodID get_class_loader = env->GetMethodID(activity_class,"getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(FMODBridge_dmGraphics_GetNativeAndroidActivity(), get_class_loader);
    jclass class_loader = env->FindClass("java/lang/ClassLoader");
    jmethodID find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring str_class_name = env->NewStringUTF(classname);
    jclass outcls = (jclass)env->CallObjectMethod(cls, find_class, str_class_name);
    env->DeleteLocalRef(str_class_name);
    return outcls;
}

static void jniLogException(JNIEnv* env) {
    jthrowable e = env->ExceptionOccurred();
    env->ExceptionClear();

    jclass clazz = env->GetObjectClass(e);
    jmethodID getMessage = env->GetMethodID(clazz, "getMessage", "()Ljava/lang/String;");
    jstring message = (jstring)env->CallObjectMethod(e, getMessage);
    const char *mstr = env->GetStringUTFChars(message, NULL);
    LOGE("%s", mstr);
    env->ReleaseStringUTFChars(message, mstr);
    env->DeleteLocalRef(message);
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(e);
}

bool FMODBridge::linkLibraries() {
    FMODBridge::AttachScope jniScope;
    JNIEnv* env = jniScope.env;

    // Load the FMOD Java Lib

    jclass helperClass = jniGetClass(env, "me.petcu.fmodbridge.BridgeHelper");
    jmethodID helperLoadMethod = env->GetStaticMethodID(helperClass, "loadFMOD", "()V");
    env->CallStaticVoidMethod(helperClass, helperLoadMethod);

    if (env->ExceptionCheck()) {
        jniLogException(env);
        return false;
    }

    // Initialize the FMOD Java lib

    jclass fmodClass = jniGetClass(env, "org.fmod.FMOD");
    jmethodID initMethod = env->GetStaticMethodID(fmodClass, "init", "(Landroid/content/Context;)V");
    if (env->ExceptionCheck()) {
        jniLogException(env);
        return false;
    }
    env->CallStaticVoidMethod(fmodClass, initMethod, FMODBridge_dmGraphics_GetNativeAndroidActivity());

    if (env->ExceptionCheck()) {
        jniLogException(env);
        return false;
    }

    // Get paths to libfmod.so and libfmodstudio.so

    jclass systemClass = jniGetClass(env, "java.lang.System");
    jmethodID mapLibMethod = env->GetStaticMethodID(systemClass, "mapLibraryName", "(Ljava/lang/String;)Ljava/lang/String;");

    jstring fmodString = env->NewStringUTF("fmod");
    jstring fmodLibPath = (jstring)env->CallStaticObjectMethod(systemClass, mapLibMethod, fmodString);
    env->DeleteLocalRef(fmodString);
    const char *fmodLibPathStr = env->GetStringUTFChars(fmodLibPath, NULL);

    jstring fmodStudioString = env->NewStringUTF("fmodstudio");
    jstring fmodStudioLibPath = (jstring)env->CallStaticObjectMethod(systemClass, mapLibMethod, fmodStudioString);
    env->DeleteLocalRef(fmodStudioString);
    const char *fmodStudioLibPathStr = env->GetStringUTFChars(fmodStudioLibPath, NULL);

    // Get dlopen handles to libfmod.so and libfmodstudio.so

    dlHandleLL = dlopen(fmodLibPathStr, RTLD_NOW | RTLD_GLOBAL);
    if (!dlHandleLL) { LOGW("%s", dlerror()); }

    dlHandleST = dlopen(fmodStudioLibPathStr, RTLD_NOW | RTLD_GLOBAL);
    if (!dlHandleST) { LOGW("%s", dlerror()); }

    env->ReleaseStringUTFChars(fmodLibPath, fmodLibPathStr);
    env->DeleteLocalRef(fmodLibPath);
    env->ReleaseStringUTFChars(fmodStudioLibPath, fmodStudioLibPathStr);
    env->DeleteLocalRef(fmodStudioLibPath);

    bool result = dlHandleLL && dlHandleST;
    if (!result) {
        cleanupLibraries();
    }

    return result;
}

void FMODBridge::cleanupLibraries() {
    FMODBridge::AttachScope jniScope;
    JNIEnv* env = jniScope.env;

    jclass fmodClass = jniGetClass(env, "org.fmod.FMOD");
    jmethodID closeMethod = env->GetStaticMethodID(fmodClass, "close", "(V)V");
    env->CallStaticVoidMethod(fmodClass, closeMethod);

    if (env->ExceptionCheck()) {
        jniLogException(env);
    }
}

#else
bool FMODBridge::linkLibraries() {
    if (FMODBridge::dlHandleLL && FMODBridge::dlHandleST) {
        return true;
    }

    #ifdef _WIN32
    if (FMODBridge::dlHandleST) { FreeLibrary(dlHandleST); }
    if (FMODBridge::dlHandleLL) { FreeLibrary(dlHandleLL); }
    #else
    if (FMODBridge::dlHandleST) { dlclose(dlHandleST); }
    if (FMODBridge::dlHandleLL) { dlclose(dlHandleLL); }
    #endif

    char *exePath = NULL;
    const char *libPath = ".";
    bool mustFreeLibPath = false;

    char* env = NULL;
    #if defined(__linux__)
    env = secure_getenv("DEFOLD_FMOD_LIB_PATH");
    #elif !defined(_WIN32)
    env = getenv("DEFOLD_FMOD_LIB_PATH");
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

        #elif defined(__linux__)
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

        #elif defined(_WIN32)
        exePath = new char[MAX_PATH];
        size_t ret = GetModuleFileNameA(GetModuleHandle(NULL), exePath, MAX_PATH);
        if (ret > 0 && ret < MAX_PATH) {
            char* newPath = new char[MAX_PATH];
            strcpy(newPath, exePath);
            dirname(newPath);
            libPath = newPath;
            mustFreeLibPath = true;
        } else {
            exePath[0] = 0;
        }
        #endif

        // Detect if the game is running in the editor

        #if defined(__APPLE__)
        #define FMB_PLATFORM "osx"
        #define FMB_EXT ""
        #elif defined(__linux__)
        #define FMB_PLATFORM "linux"
        #define FMB_EXT ""
        #elif defined(_WIN32)
        #define FMB_PLATFORM "win32"
        #define FMB_EXT ".exe"
        #endif

        #if defined(__x86_64__) || defined(_M_X64)
        #define FMB_ARCH "x86_64"
        #elif defined(__i386) || defined(_M_IX86)
        #define FMB_ARCH "x86"
        #endif

        #if defined(FMB_PLATFORM) && defined(FMB_ARCH)
        #define FMB_EDITOR_SUFFIX SEP "build" SEP FMB_ARCH "-" FMB_PLATFORM SEP "dmengine" FMB_EXT

        static const size_t suffixLen = strlen(FMB_EDITOR_SUFFIX);
        size_t exePathLen = strlen(exePath);
        if (exePathLen >= suffixLen && 0 == strcmp(FMB_EDITOR_SUFFIX, exePath + exePathLen - suffixLen)) {
            LOGI("Running in the editor. Will attempt to load libraries from project");

            const char* projPath = dirname(dirname(dirname(exePath)));
            const char* resPath = FMODBridge_dmConfigFile_GetString("fmod.lib_path", "");

            if (!resPath[0]) {
                LOGW("fmod.lib_path not found in game.project. See README for details");
            }

            #ifdef __APPLE__
            #define FMB_LIB_PATH SEP FMB_ARCH "-" FMB_PLATFORM SEP "Contents" SEP "MacOS"
            #else
            #define FMB_LIB_PATH SEP FMB_ARCH "-" FMB_PLATFORM
            #endif

            size_t projPathLen = strlen(projPath);
            size_t resPathLen = strlen(resPath);
            size_t libPathLen = strlen(FMB_LIB_PATH);
            size_t len = 0;
            char* newPath = new char[projPathLen + 1 + resPathLen + libPathLen + 1];

            strcpy(newPath, projPath);
            len += projPathLen;

            if (resPath[0] != '/') {
                strcat(newPath, SEP);
                len += 1;
            }

            strcat(newPath + len, resPath);
            #ifdef _WIN32
            for (size_t i = len; i < len + resPathLen; i++) {
                if (newPath[i] == '/') { newPath[i] = SEPCH; }
            }
            #endif
            len += resPathLen;

            strcat(newPath + len, FMB_LIB_PATH);

            if (mustFreeLibPath) { delete[] libPath; }
            libPath = newPath;
            mustFreeLibPath = true;
        }

        #endif
    }

    #if defined(__APPLE__)
        #define LIBEXT "dylib"
    #elif defined(_WIN32)
        #define LIBEXT "dll"
    #else
        #define LIBEXT "so"
    #endif

    #ifdef _WIN32
        #define LIBPREFIX ""
        #if defined(__x86_64__) || defined(_M_X64)
            #define LIBPOSTFIX "64"
        #else
            #define LIBPOSTFIX ""
        #endif
        #define libOpen(var, path) \
            var = LoadLibraryA(path); \
            if (!var) { LOGW("LoadLibrary(\"%s\") failed with error code %lu", path, GetLastError()); }
    #else
        #define LIBPREFIX "lib"
        #define LIBPOSTFIX ""
        #define libOpen(var, path) \
            var = dlopen(path, RTLD_NOW | RTLD_GLOBAL); \
            if (!var) { LOGW("%s", dlerror()); }
    #endif

    if (exePath) { delete[] exePath; }
    size_t maxPathLen = strlen(libPath) + 20;
    exePath = new char[maxPathLen + 1];

    strcpy(exePath, libPath);
    strncat(exePath, SEP LIBPREFIX "fmod" LIBPOSTFIX "." LIBEXT, maxPathLen);
    libOpen(FMODBridge::dlHandleLL, exePath);

    strcpy(exePath, libPath);
    strncat(exePath, SEP LIBPREFIX "fmodstudio" LIBPOSTFIX "." LIBEXT, maxPathLen);
    libOpen(FMODBridge::dlHandleST, exePath);

    if (mustFreeLibPath) { delete[] libPath; }
    delete[] exePath;
    return (FMODBridge::dlHandleLL && FMODBridge::dlHandleST);
}

void FMODBridge::cleanupLibraries() {
}
#endif
#endif
