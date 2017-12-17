#ifndef FMOD_HELPERS_H
#define FMOD_HELPERS_H

#include <dmsdk/sdk.h>
#include <LuaBridge/LuaBridge.h>
#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <map>
#include "fmod_bridge.hpp"

namespace FMODBridge {
    inline void errCheck_(FMOD_RESULT res, lua_State* L) {
        if (res != FMOD_OK) {
            lua_pushstring(L, FMOD_ErrorString(res));
            lua_error(L);
        }
    }

    extern std::map<void*, int> refCounts;

    template<class T>
    class RefCountedProxy {
    protected:
        T* instance;
    public:
        RefCountedProxy(T* instance_): instance(instance_) {
            refCounts[instance_] += 1;
        }
        RefCountedProxy(const RefCountedProxy<T>& other): instance(other.instance) {
            refCounts[instance] += 1;
        }
        ~RefCountedProxy() {
            int count = refCounts[instance];
            if (count <= 1) {
                if (FMODBridge::system) { instance->release(); }
                refCounts.erase(instance);
            } else {
                refCounts[instance] = count - 1;
            }
        }
    };

}

#define errCheck(res) FMODBridge::errCheck_(res, L)

#define makeStringGetter(instance, fname) \
    std::string fname(lua_State* L) { \
        int size; \
        errCheck(instance->fname(NULL, 0, &size)); \
        char* tmp = new char[size]; \
        errCheck(instance->getPath(tmp, size, NULL)); \
        std::string str(tmp); \
        delete[] tmp; \
        return str; \
    }

#define makeCastGetter(RT, T, instance, fname) \
    RT fname(lua_State* L) { \
        T result; \
        errCheck(instance->fname(&result)); \
        return result; \
    }

#define makeCastGetter1(RT, T, instance, fname, T1) \
    RT fname(T1 arg1, lua_State* L) { \
        T result; \
        errCheck(instance->fname(arg1, &result)); \
        return result; \
    }

#define makeCastGetter2(RT, T, instance, fname, T1, T2) \
    RT fname(T1 arg1, T2 arg2, lua_State* L) { \
        T result; \
        errCheck(instance->fname(arg1, arg2, &result)); \
        return result; \
    }

#define makeValueGetter(T, instance, fname) makeCastGetter(T, T, instance, fname)
#define makeValueGetter1(T, instance, fname, T1) makeCastGetter1(T, T, instance, fname, T1)
#define makeValueGetter2(T, instance, fname, T1, T2) makeCastGetter2(T, T, instance, fname, T1, T2)

#define makeMethod(instance, fname) \
    void fname(lua_State* L) { \
        errCheck(instance->fname()); \
    }

#define makeMethod1(instance, fname, T1) \
    void fname(T1 arg1, lua_State* L) { \
        errCheck(instance->fname(arg1)); \
    }

#define makeMethod2(instance, fname, T1, T2) \
    void fname(T1 arg1, T2 arg2, lua_State* L) { \
        errCheck(instance->fname(arg1, arg2)); \
    }

namespace luabridge {
    template <>
    struct Stack <dmScript::LuaHBuffer*> {
      static void push (lua_State* L, dmScript::LuaHBuffer* luaBuffer) {
          dmScript::PushBuffer(L, *luaBuffer);
      }

      static dmScript::LuaHBuffer* get(lua_State* L, int index) {
          return dmScript::CheckBuffer(L, index);
      }
    };

    template <>
    struct Stack <FMOD_VECTOR> {
      static void push (lua_State* L, FMOD_VECTOR vec) {
          lua_pushstring(L, "vmath");
          lua_gettable(L, LUA_GLOBALSINDEX);
          lua_pushstring(L, "vector3");
          lua_gettable(L, -2);

          lua_pushnumber(L, vec.x);
          lua_pushnumber(L, vec.y);
          lua_pushnumber(L, vec.z);
          lua_call(L, 3, 1);

          lua_remove(L, -2);
      }

      static FMOD_VECTOR get(lua_State* L, int index) {
          FMOD_VECTOR vec;

          lua_pushstring(L, "x");
          lua_gettable(L, index);
          vec.x = luaL_checknumber(L, -1);
          lua_pop(L, 1);

          lua_pushstring(L, "y");
          lua_gettable(L, index);
          vec.y = luaL_checknumber(L, -1);
          lua_pop(L, 1);

          lua_pushstring(L, "z");
          lua_gettable(L, index);
          vec.z = luaL_checknumber(L, -1);
          lua_pop(L, 1);

          return vec;
      }
    };
}

#endif