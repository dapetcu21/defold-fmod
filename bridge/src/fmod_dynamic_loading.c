#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "fmod_bridge.h"
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
#include <linux/limits.h>
#endif

dlModuleT FMODBridge_dlHandleLL = NULL;
dlModuleT FMODBridge_dlHandleST = NULL;

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

static JNIEnv* FMODBridge_jni_env = NULL;
static int FMODBridge_jni_refcount = 0;

void FMODBridge_attachJNI()
{
    if (FMODBridge_jni_refcount == 0) {
        JavaVM* vm = FMODBridge_dmGraphics_GetNativeAndroidJavaVM();
        (*vm)->AttachCurrentThread(vm, &FMODBridge_jni_env, NULL);
    }
    FMODBridge_jni_refcount += 1;
}

void FMODBridge_detachJNI()
{
    FMODBridge_jni_refcount -= 1;
    if (FMODBridge_jni_refcount == 0) {
        JNIEnv* env = FMODBridge_jni_env;
        bool exception = (bool)(*env)->ExceptionCheck(env);
        (*env)->ExceptionClear(env);
        JavaVM* vm = FMODBridge_dmGraphics_GetNativeAndroidJavaVM();
        (*vm)->DetachCurrentThread(vm);
    }
}

static jclass jniGetClass(JNIEnv* env, const char* classname) {
    jclass activity_class = (*env)->FindClass(env, "android/app/NativeActivity");
    jmethodID get_class_loader = (*env)->GetMethodID(env, activity_class,"getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = (*env)->CallObjectMethod(env, FMODBridge_dmGraphics_GetNativeAndroidActivity(), get_class_loader);
    jclass class_loader = (*env)->FindClass(env, "java/lang/ClassLoader");
    jmethodID find_class = (*env)->GetMethodID(env, class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring str_class_name = (*env)->NewStringUTF(env, classname);
    jclass outcls = (jclass)(*env)->CallObjectMethod(env, cls, find_class, str_class_name);
    (*env)->DeleteLocalRef(env, str_class_name);
    return outcls;
}

static void jniLogException(JNIEnv* env) {
    jthrowable e = (*env)->ExceptionOccurred(env);
    (*env)->ExceptionClear(env);

    jclass clazz = (*env)->GetObjectClass(env, e);
    jmethodID getMessage = (*env)->GetMethodID(env, clazz, "getMessage", "()Ljava/lang/String;");
    jstring message = (jstring)(*env)->CallObjectMethod(env, e, getMessage);
    const char *mstr = (*env)->GetStringUTFChars(env, message, NULL);
    LOGE("%s", mstr);
    (*env)->ReleaseStringUTFChars(env, message, mstr);
    (*env)->DeleteLocalRef(env, message);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, e);
}

bool FMODBridge_linkLibraries() {
    attachJNI();
    JNIEnv* env = FMODBridge_jni_env;

    // Load the FMOD Java Lib

    jclass helperClass = jniGetClass(env, "me.petcu.fmodbridge.BridgeHelper");
    jmethodID helperLoadMethod = (*env)->GetStaticMethodID(env, helperClass, "loadFMOD", "()V");
    (*env)->CallStaticVoidMethod(env, helperClass, helperLoadMethod);

    if ((*env)->ExceptionCheck(env)) {
        jniLogException(env);
        detachJNI();
        return false;
    }

    // Initialize the FMOD Java lib

    jclass fmodClass = jniGetClass(env, "org.fmod.FMOD");
    jmethodID initMethod = (*env)->GetStaticMethodID(env, fmodClass, "init", "(Landroid/content/Context;)V");
    if ((*env)->ExceptionCheck(env)) {
        jniLogException(env);
        detachJNI();
        return false;
    }
    (*env)->CallStaticVoidMethod(env, fmodClass, initMethod, FMODBridge_dmGraphics_GetNativeAndroidActivity());

    if ((*env)->ExceptionCheck(env)) {
        jniLogException(env);
        detachJNI();
        return false;
    }

    // Get paths to libfmod.so and libfmodstudio.so

    jclass systemClass = jniGetClass(env, "java.lang.System");
    jmethodID mapLibMethod = (*env)->GetStaticMethodID(env, systemClass, "mapLibraryName", "(Ljava/lang/String;)Ljava/lang/String;");

    jstring fmodString = (*env)->NewStringUTF(env, "fmod");
    jstring fmodLibPath = (jstring)(*env)->CallStaticObjectMethod(env, systemClass, mapLibMethod, fmodString);
    (*env)->DeleteLocalRef(env, fmodString);
    const char *fmodLibPathStr = (*env)->GetStringUTFChars(env, fmodLibPath, NULL);

    jstring fmodStudioString = (*env)->NewStringUTF(env, "fmodstudio");
    jstring fmodStudioLibPath = (jstring)(*env)->CallStaticObjectMethod(env, systemClass, mapLibMethod, fmodStudioString);
    (*env)->DeleteLocalRef(env, fmodStudioString);
    const char *fmodStudioLibPathStr = (*env)->GetStringUTFChars(env, fmodStudioLibPath, NULL);

    // Get dlopen handles to libfmod.so and libfmodstudio.so

    FMODBridge_dlHandleLL = dlopen(fmodLibPathStr, RTLD_NOW | RTLD_GLOBAL);
    if (!FMODBridge_dlHandleLL) { LOGW("%s", dlerror()); }

    FMODBridge_dlHandleST = dlopen(fmodStudioLibPathStr, RTLD_NOW | RTLD_GLOBAL);
    if (!FMODBridge_dlHandleST) { LOGW("%s", dlerror()); }

    (*env)->ReleaseStringUTFChars(env, fmodLibPath, fmodLibPathStr);
    (*env)->DeleteLocalRef(env, fmodLibPath);
    (*env)->ReleaseStringUTFChars(env, fmodStudioLibPath, fmodStudioLibPathStr);
    (*env)->DeleteLocalRef(env, fmodStudioLibPath);

    bool result = FMODBridge_dlHandleLL && FMODBridge_dlHandleST;
    if (!result) {
        FMODBridge_cleanupLibraries();
    }

    detachJNI();
    return result;
}

void FMODBridge_cleanupLibraries() {
    attachJNI();
    JNIEnv* env = FMODBridge_jni_env;

    jclass fmodClass = jniGetClass(env, "org.fmod.FMOD");
    jmethodID closeMethod = (*env)->GetStaticMethodID(env, fmodClass, "close", "()V");
    (*env)->CallStaticVoidMethod(env, fmodClass, closeMethod);

    if ((*env)->ExceptionCheck(env)) {
        jniLogException(env);
    }

    detachJNI();
}

#else

static bool endsIn(const char * haystack, const char * needle) {
    size_t needleLen = strlen(needle);
    size_t haystackLen = strlen(haystack);
    return (haystackLen >= needleLen && 0 == strcmp(needle, haystack + haystackLen - needleLen));
}

bool FMODBridge_linkLibraries() {
    if (FMODBridge_dlHandleLL && FMODBridge_dlHandleST) {
        return true;
    }

    #ifdef _WIN32
    if (FMODBridge_dlHandleST) { FreeLibrary(FMODBridge_dlHandleST); }
    if (FMODBridge_dlHandleLL) { FreeLibrary(FMODBridge_dlHandleLL); }
    #else
    if (FMODBridge_dlHandleST) { dlclose(FMODBridge_dlHandleST); }
    if (FMODBridge_dlHandleLL) { dlclose(FMODBridge_dlHandleLL); }
    #endif

    char *exePath = NULL;
    const char *libPath = ".";
    bool mustFreeLibPath = false;

    char* env = NULL;
    #if defined(_GNU_SOURCE)
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
        exePath = (char*)malloc(bufsize);
        _NSGetExecutablePath(exePath, &bufsize);
        libPath = dirname(exePath);
        char* newPath = (char*)malloc(strlen(libPath) + 1);
        strcpy(newPath, libPath);
        libPath = newPath;
        mustFreeLibPath = true;

        #elif defined(__linux__)
        exePath = (char*)malloc(PATH_MAX + 2);
        ssize_t ret = readlink("/proc/self/exe", exePath, PATH_MAX + 2);
        if (ret >= 0 && ret <= PATH_MAX + 1) {
          exePath[ret] = 0;
          char* newPath = (char*)malloc(ret + 1);
          strcpy(newPath, exePath);
          libPath = dirname(newPath); // dirname() clobbers newPath
          char* finalPath = (char*)malloc(ret + 1);
          strcpy(finalPath, libPath);
          libPath = finalPath;
          mustFreeLibPath = true;
          free(newPath);
        } else {
          exePath[0] = 0;
        }

        #elif defined(_WIN32)
        exePath = (char*)malloc(MAX_PATH);
        size_t ret = GetModuleFileNameA(GetModuleHandle(NULL), exePath, MAX_PATH);
        if (ret > 0 && ret < MAX_PATH) {
            char* newPath = (char*)malloc(MAX_PATH);
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
        #define FMB_PLATFORM_ALT "darwin"
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

        #ifdef FMB_PLATFORM_ALT
        #define FMB_EDITOR_SUFFIX_ALT SEP "build" SEP FMB_ARCH "-" FMB_PLATFORM_ALT SEP "dmengine" FMB_EXT
        if (endsIn(exePath, FMB_EDITOR_SUFFIX) || endsIn(exePath, FMB_EDITOR_SUFFIX_ALT)) {
        #else
        if (endsIn(exePath, FMB_EDITOR_SUFFIX)) {
        #endif
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
            char* newPath = (char*)malloc(projPathLen + 1 + resPathLen + libPathLen + 1);

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

            if (mustFreeLibPath) { free((void*)libPath); }
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
        #define libOpen(var, path) \
            var = LoadLibraryA(path); \
            if (!var) { LOGW("LoadLibrary(\"%s\") failed with error code %lu", path, GetLastError()); }
    #else
        #define LIBPREFIX "lib"
        #define libOpen(var, path) \
            var = dlopen(path, RTLD_NOW | RTLD_GLOBAL); \
            if (!var) { LOGW("%s", dlerror()); }
    #endif

    if (exePath) { free(exePath); }
    size_t maxPathLen = strlen(libPath) + 20;
    exePath = (char*)malloc(maxPathLen + 1);

    strcpy(exePath, libPath);
    strncat(exePath, SEP LIBPREFIX "fmod." LIBEXT, maxPathLen);
    libOpen(FMODBridge_dlHandleLL, exePath);

    strcpy(exePath, libPath);
    strncat(exePath, SEP LIBPREFIX "fmodstudio." LIBEXT, maxPathLen);
    libOpen(FMODBridge_dlHandleST, exePath);

    if (mustFreeLibPath) { free((void*)libPath); }
    free(exePath);
    return (FMODBridge_dlHandleLL && FMODBridge_dlHandleST);
}

void FMODBridge_cleanupLibraries() {
}
#endif
#endif
