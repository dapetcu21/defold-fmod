#ifndef FMOD_HELPERS_H
#define FMOD_HELPERS_H

#include <dmsdk/sdk.h>
#include <LuaBridge/LuaBridge.h>
#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <fmod_errors.h>

namespace FMODBridge {
    inline void errCheck_(FMOD_RESULT res, lua_State* L) {
        if (res != FMOD_OK) {
            lua_pushstring(L, FMOD_ErrorString(res));
            lua_error(L);
        }
    }
}

#define errCheck(res) FMODBridge::errCheck_(res, L)

#define stringGetter(instance, getterName) \
    std::string getterName(lua_State* L) { \
        int size; \
        errCheck(instance->getterName(NULL, 0, &size)); \
        char* tmp = new char[size]; \
        errCheck(instance->getPath(tmp, size, NULL)); \
        std::string str(tmp); \
        delete[] tmp; \
        return str; \
    }

template <>
struct luabridge::Stack <dmScript::LuaHBuffer*> {
  static void push (lua_State* L, dmScript::LuaHBuffer* luaBuffer) {
      dmScript::PushBuffer(L, *luaBuffer);
  }

  static dmScript::LuaHBuffer* get(lua_State* L, int index) {
      return dmScript::CheckBuffer(L, index);
  }
};

#endif