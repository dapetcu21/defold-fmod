#include <string.h>
#include "fmod_bridge.hpp"
#include <LuaBridge/LuaBridge.h>

#define LIGHT_STRUCT (1 << 0)

typedef struct StructUserdata {
    int flags;
    char data[];
} StructUserdata;

static void * pushStruct(lua_State *L, const void * structData, size_t structSize, int registryIndex) {
    StructUserdata * userdata = (StructUserdata*)lua_newuserdata(L, sizeof(StructUserdata) + structSize);
    userdata->flags = 0;
    if (structData) {
        memcpy(userdata->data, structData, structSize);
    } else {
        memset(userdata->data, 0, structSize);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, registryIndex);
    lua_setmetatable(L, -2);
    return userdata->data;
}

static void * pushStructLight(lua_State *L, void * structData, int registryIndex) {
    StructUserdata * userdata = (StructUserdata*)lua_newuserdata(L, sizeof(StructUserdata) + sizeof(void*));
    userdata->flags = LIGHT_STRUCT;
    *((void**)(&(userdata->data[0]))) = structData;

    lua_rawgeti(L, LUA_REGISTRYINDEX, registryIndex);
    lua_setmetatable(L, -2);
    return userdata;
}

static void * checkStruct(lua_State *L, int index, int registryIndex, const char * structName) {
    StructUserdata * userdata = (StructUserdata*)lua_touserdata(L, index);
    lua_rawgeti(L, LUA_REGISTRYINDEX, registryIndex);
    if (userdata == NULL || !lua_getmetatable(L, index) || !lua_rawequal(L, -1, -2)) {
        luaL_typerror(L, index, structName);
    }
    lua_pop(L, 2);
    if (userdata->flags & LIGHT_STRUCT) {
        return *((void**)(&(userdata->data[0])));
    }
    return userdata->data;
}

static int structConstructor(lua_State *L) {
    int registryIndex = (int)lua_tonumber(L, lua_upvalueindex(1));
    size_t structSize = (size_t)lua_tonumber(L, lua_upvalueindex(2));
    pushStruct(L, NULL, structSize, registryIndex);
    return 1;
}

#define declareStructAccessors(structName) \
    static int CONCAT(FMODBridge_registry_, structName) = LUA_REFNIL; \
    static inline structName * CONCAT(FMODBridge_push_, structName)(lua_State *L, const structName * structData) { \
        return (structName*)pushStruct(L, structData, sizeof(structName), CONCAT(FMODBridge_registry_, structName)); \
    } \
    static inline structName * CONCAT(FMODBridge_pushLight_, structName)(lua_State *L, structName * structData) { \
        return (structName*)pushStructLight(L, structData, CONCAT(FMODBridge_registry_, structName)); \
    } \
    static inline structName * CONCAT(FMODBridge_check_, structName)(lua_State *L, int index) { \
        return (structName*)checkStruct(L, index, CONCAT(FMODBridge_registry_, structName), STRINGIFY(structName)); \
    }

{% for struct in structs
    %}declareStructAccessors({{ struct[0] }});
{% endfor %}

extern "C" void FMODBridge_registerClasses(lua_State *L) {
}

extern "C" void FMODBridge_registerEnums(lua_State *L) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "fmod");

    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, "studio");

    #define addEnum(x) \
        lua_pushstring(L, #x); \
        lua_pushnumber(L, (lua_Number)CONCAT(FMOD_, x)); \
        lua_rawset(L, -4)

    {% for enum in enums
        %}addEnum({{ enum }});
    {% endfor %}

    #define addStruct(structName, name, containerIndex) \
        lua_newtable(L); \
        lua_pushvalue(L, -1); \
        CONCAT(FMODBridge_registry_, structName) = luaL_ref(L, LUA_REGISTRYINDEX); \
        \
        lua_pushnumber(L, CONCAT(FMODBridge_registry_, structName)); \
        lua_pushnumber(L, sizeof(structName)); \
        lua_pushcclosure(L, &structConstructor, 2); \
        lua_setfield(L, containerIndex - 2, name)

    {% for struct in structs
        %}addStruct({{ struct[0] }}, "{{ struct[1][0] }}", {{ struct[1][1] }}); lua_pop(L, 1);
    {% endfor %}

    lua_pop(L, 2);
}
