#ifndef FMOD_HELPERS_H
#define FMOD_HELPERS_H

#include "fmod_bridge.hpp"
#include <LuaBridge/LuaBridge.h>
#include <fmod_errors.h>
#include <map>
#include <string>

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
        typedef FMOD_RESULT (F_CALL *ReleaseFunc)(T*);
        ReleaseFunc release;
    public:
        RefCountedProxy(T* instance_): instance(instance_), release(NULL) {
            refCounts[instance_] += 1;
        }
        RefCountedProxy(const RefCountedProxy<T> &other): instance(other.instance), release(other.release) {
            refCounts[instance] += 1;
        }
        operator T*() const { return instance; }
        ~RefCountedProxy() {
            int count = refCounts[instance];
            if (count <= 1) {
                if (FMODBridge::system) { release(instance); }
                refCounts.erase(instance);
            } else {
                refCounts[instance] = count - 1;
            }
        }
    };

    #define makeProxyConstructor(className, typeName) \
        className(typeName* instance): RefCountedProxy(instance) { \
            ensure(currentLib, makeFname(Release), FMOD_RESULT, typeName*); \
            release = makeFname(Release); \
        }

    template<class T>
    class Proxy {
    protected:
        T* instance;
    public:
        Proxy(T* instance_): instance(instance_) {}
        Proxy(const Proxy<T>& other): instance(other.instance) {}
        operator T*() const { return instance; }
    };

    template<typename RT, typename T>
    void pushList(lua_State* L, T** list, int count) {
        lua_createtable(L, count, 0);
        for (int i = 0; i < count; i++) {
            luabridge::Stack<RT>::push(L, RT(list[i]));
            lua_rawseti(L, -2, i + 1);
        }
    }
}

#define errCheck(res) FMODBridge::errCheck_(res, L)

#define LIB_PREFIX_LL FMOD_
#define LIB_PREFIX_ST FMOD_Studio_
#define libPrefix(x) CONCAT(LIB_PREFIX_, x)
#define makeFname(fname) CONCAT(libPrefix(currentLib), CONCAT(CONCAT(currentClass, _), fname))

#define defineStringGetter(declname, fname) \
    std::string declname(lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, const char*, int, int*); \
        int size; \
        errCheck(makeFname(fname)(instance, NULL, 0, &size)); \
        char* tmp = new char[size]; \
        errCheck(makeFname(fname)(instance, tmp, size, NULL)); \
        std::string str(tmp); \
        delete[] tmp; \
        return str; \
    }

#define defineStringGetter1(declname, fname, T1) \
    std::string declname(T1 arg1, lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T1, const char*, int, int*); \
        int size; \
        errCheck(makeFname(fname)(instance, arg1, NULL, 0, &size)); \
        char* tmp = new char[size]; \
        errCheck(makeFname(fname)(instance, arg1, tmp, size, NULL)); \
        std::string str(tmp); \
        delete[] tmp; \
        return str; \
    }

#define makeStringGetter(fname) defineStringGetter(fname, fname)
#define makeStringGetter1(fname, T1) defineStringGetter1(fname, fname, T1)

#define defineCastGetter(declname, fname, RT, T) \
    RT declname(lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T*); \
        T result; \
        errCheck(makeFname(fname)(instance, &result)); \
        return result; \
    }

#define defineCastGetter1(declname, fname, RT, T, T1) \
    RT declname(T1 arg1, lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T1, T*); \
        T result; \
        errCheck(makeFname(fname)(instance, arg1, &result)); \
        return result; \
    }

#define defineCastGetter2(declname, fname, RT, T, T1, T2) \
    RT declname(T1 arg1, T2 arg2, lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T1, T2, T*); \
        T result; \
        errCheck(makeFname(fname)(instance, arg1, arg2, &result)); \
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
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*); \
        errCheck(makeFname(fname)(instance)); \
    }

#define defineMethod1(declname, fname, T1) \
    void declname(T1 arg1, lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T1); \
        errCheck(makeFname(fname)(instance, arg1)); \
    }

#define defineMethod2(declname, fname, T1, T2) \
    void declname(T1 arg1, T2 arg2, lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T1, T2); \
        errCheck(makeFname(fname)(instance, arg1, arg2)); \
    }

#define makeMethod(fname) defineMethod(fname, fname)
#define makeMethod1(fname, T1) defineMethod1(fname, fname, T1)
#define makeMethod2(fname, T1, T2) defineMethod2(fname, fname, T1, T2)

#define define2FloatGetter(declname, fname) \
    int declname(lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, float*, float*); \
        float value, finalValue; \
        errCheck(makeFname(fname)(instance, &value, &finalValue)); \
        lua_pushnumber(L, value); \
        lua_pushnumber(L, finalValue); \
        return 2; \
    }

#define define2FloatGetter1(declname, fname, T1) \
    int declname(lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T1, float*, float*); \
        T1 arg1 = luabridge::Stack<T1>::get(L, 2); \
        float value, finalValue; \
        errCheck(makeFname(fname)(instance, arg1, &value, &finalValue)); \
        lua_pushnumber(L, value); \
        lua_pushnumber(L, finalValue); \
        return 2; \
    }

#define define2FloatGetter2(declname, fname, T1, T2) \
    int declname(lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T1, T2, float*, float*); \
        T1 arg1 = luabridge::Stack<T1>::get(L, 2); \
        T2 arg2 = luabridge::Stack<T2>::get(L, 3); \
        float value, finalValue; \
        errCheck(makeFname(fname)(instance, arg1, arg2, &value, &finalValue)); \
        lua_pushnumber(L, value); \
        lua_pushnumber(L, finalValue); \
        return 2; \
    }

#define make2FloatGetter(fname) define2FloatGetter(fname, fname)
#define make2FloatGetter1(fname, T1) define2FloatGetter1(fname, fname, T1)
#define make2FloatGetter2(fname, T1, T2) define2FloatGetter2(fname, fname, T1, T2)

#define defineListGetter(declname, fname, RT, T, GetCount) \
    int declname(lua_State* L) { \
        ensure(currentLib, makeFname(fname), FMOD_RESULT, currentType*, T**, int, int*); \
        int capacity = GetCount(L); \
        int count; \
        T** array = new T*[capacity]; \
        errCheck(makeFname(fname)(instance, array, capacity, &count)); \
        FMODBridge::pushList<RT, T>(L, array, count); \
        delete[] array; \
        return 1; \
    }

#define makeListGetter(fname, RT, T, GetCount) defineListGetter(fname, fname, RT, T, GetCount)

#define declareEnum(EnumT) \
template <> \
struct luabridge::Stack <EnumT> { \
  static void push(lua_State* L, EnumT value) { \
      luabridge::Stack<int>::push(L, value); \
  } \
  static EnumT get(lua_State* L, int index) { \
      return (EnumT)luabridge::Stack<int>::get(L, index); \
  } \
};

namespace luabridge {
    template <>
    struct Stack <FMODBridge::LuaHBuffer*> {
      static void push(lua_State* L, FMODBridge::LuaHBuffer* luaBuffer) {
          FMODBridge_dmScript_PushBuffer(L, luaBuffer->handle);
      }

      static FMODBridge::LuaHBuffer* get(lua_State* L, int index) {
          static FMODBridge::LuaHBuffer wrapper;
          wrapper.handle = FMODBridge_dmScript_CheckBuffer(L, index);
          return &wrapper;
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
          vec.x = (float)luaL_checknumber(L, -1);
          lua_pop(L, 1);

          lua_pushstring(L, "y");
          lua_gettable(L, index);
          vec.y = (float)luaL_checknumber(L, -1);
          lua_pop(L, 1);

          lua_pushstring(L, "z");
          lua_gettable(L, index);
          vec.z = (float)luaL_checknumber(L, -1);
          lua_pop(L, 1);

          return vec;
      }
    };
}

#endif