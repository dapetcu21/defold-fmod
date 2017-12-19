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
        operator T*() const { return instance; }
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

    template<class T>
    class Proxy {
    protected:
        T* instance;
    public:
        Proxy(T* instance_): instance(instance_) {}
        Proxy(const Proxy<T>& other): instance(other.instance) {}
        operator T*() const { return instance; }
    };
}

#define errCheck(res) FMODBridge::errCheck_(res, L)

#define defineStringGetter(declname, fname) \
    std::string declname(lua_State* L) { \
        int size; \
        errCheck(instance->fname(NULL, 0, &size)); \
        char* tmp = new char[size]; \
        errCheck(instance->fname(tmp, size, NULL)); \
        std::string str(tmp); \
        delete[] tmp; \
        return str; \
    }

#define defineStringGetter1(declname, fname, T1) \
    std::string declname(T1 arg1, lua_State* L) { \
        int size; \
        errCheck(instance->fname(arg1, NULL, 0, &size)); \
        char* tmp = new char[size]; \
        errCheck(instance->fname(arg1, tmp, size, NULL)); \
        std::string str(tmp); \
        delete[] tmp; \
        return str; \
    }

#define makeStringGetter(fname) defineStringGetter(fname, fname)
#define makeStringGetter1(fname, T1) defineStringGetter1(fname, fname, T1)

#define defineCastGetter(declname, fname, RT, T) \
    RT declname(lua_State* L) { \
        T result; \
        errCheck(instance->fname(&result)); \
        return result; \
    }

#define defineCastGetter1(declname, fname, RT, T, T1) \
    RT declname(T1 arg1, lua_State* L) { \
        T result; \
        errCheck(instance->fname(arg1, &result)); \
        return result; \
    }

#define defineCastGetter2(declname, fname, RT, T, T1, T2) \
    RT declname(T1 arg1, T2 arg2, lua_State* L) { \
        T result; \
        errCheck(instance->fname(arg1, arg2, &result)); \
        return result; \
    }

#define defineGetter(declname, fname, T) defineCastGetter(declname, fname, T, T)
#define defineGetter1(declname, fname, T, T1) defineCastGetter1(declname, fname, T, T, T1)
#define defineGetter2(declname, fname, T, T1, T2) defineCastGetter2(declname, fname, T, T, T1, T2)

#define makeGetter(fname, T) defineGetter(fname, fname, T)
#define makeGetter1(fname, T, T1) defineGetter1(fname, fname, T, T1)
#define makeGetter2(fname, T, T1, T2) defineGetter2(fname, fname, T, T1, T2)

#define makeCastGetter(fname, RT, T) defineCastGetter(fname, fname, RT, T)
#define makeCastGetter1(fname, RT, T, T1) defineCastGetter1(fname, fname, RT, T, T1)
#define makeCastGetter2(fname, RT, T, T1, T2) defineCastGetter2(fname, fname, RT, T, T1, T2)

#define defineMethod(declname, fname) \
    void declname(lua_State* L) { \
        errCheck(instance->fname()); \
    }

#define defineMethod1(declname, fname, T1) \
    void declname(T1 arg1, lua_State* L) { \
        errCheck(instance->fname(arg1)); \
    }

#define defineMethod2(declname, fname, T1, T2) \
    void declname(T1 arg1, T2 arg2, lua_State* L) { \
        errCheck(instance->fname(arg1, arg2)); \
    }

#define makeMethod(fname) defineMethod(fname, fname)
#define makeMethod1(fname, T1) defineMethod1(fname, fname, T1)
#define makeMethod2(fname, T1, T2) defineMethod2(fname, fname, T1, T2)

#define define2FloatGetter(declname, fname) \
    int declname(lua_State* L) { \
        float value, finalValue; \
        errCheck(instance->fname(&value, &finalValue)); \
        lua_pushnumber(L, value); \
        lua_pushnumber(L, finalValue); \
        return 2; \
    }

#define define2FloatGetter1(declname, fname, T1) \
    int declname(lua_State* L) { \
        T1 arg1 = luabridge::Stack<T1>::get(L, 2); \
        float value, finalValue; \
        errCheck(instance->fname(arg1, &value, &finalValue)); \
        lua_pushnumber(L, value); \
        lua_pushnumber(L, finalValue); \
        return 2; \
    }

#define define2FloatGetter2(declname, fname, T1, T2) \
    int declname(lua_State* L) { \
        T1 arg1 = luabridge::Stack<T1>::get(L, 2); \
        T2 arg2 = luabridge::Stack<T2>::get(L, 3); \
        float value, finalValue; \
        errCheck(instance->fname(arg1, arg2, &value, &finalValue)); \
        lua_pushnumber(L, value); \
        lua_pushnumber(L, finalValue); \
        return 2; \
    }

#define make2FloatGetter(fname) define2FloatGetter(fname, fname)
#define make2FloatGetter1(fname, T1) define2FloatGetter1(fname, fname, T1)
#define make2FloatGetter2(fname, T1, T2) define2FloatGetter2(fname, fname, T1, T2)

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