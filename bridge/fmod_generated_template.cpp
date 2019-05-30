#include <string.h>
#include "fmod_bridge.hpp"
#include <LuaBridge/LuaBridge.h>

static void * pushStruct(lua_State *L, const void * structData, size_t structSize, int registryIndex) {
    void * userdata = lua_newuserdata(L, structSize);
    if (structData) {
        memcpy(userdata, structData, structSize);
    } else {
        memset(userdata, 0, structSize);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, registryIndex);
    lua_setmetatable(L, -2);
    return userdata;
}

static void * checkStruct(lua_State *L, int index, int registryIndex, const char * structName) {
    void * userdata = lua_touserdata(L, index);
    lua_rawgeti(L, LUA_REGISTRYINDEX, registryIndex);
    if (userdata == NULL || !lua_getmetatable(L, index) || !lua_rawequal(L, -1, -2)) {
        luaL_typerror(L, index, structName);
    }
    lua_pop(L, 2);
    return userdata;
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

    #define beginStruct(structName) \
        lua_newtable(L); \
        lua_pushvalue(L, -1); \
        CONCAT(FMODBridge_registry_, structName) = luaL_ref(L, LUA_REGISTRYINDEX) \
        lua_newtable(L); \
        lua_pushvalue(L, -1); \
        lua_setfield(L, -3, "_fieldget");
        lua_newtable(L); \
        lua_pushvalue(L, -1); \
        lua_setfield(L, -4, "_fieldset")

    #define endStruct() lua_pop(L, 3)

    #define addStructConstructor(structName, name, containerIndex) \
        lua_pushnumber(L, CONCAT(FMODBridge_registry_, structName)); \
        lua_pushnumber(L, sizeof(structName)); \
        lua_pushcclosure(L, &structConstructor, 2); \
        lua_setfield(L, containerIndex - 4, name)

    {% for struct in structs %}
        beginStruct({{ struct[0] }});
        addStructConstructor({{ struct[0] }}, "{{ struct[1][0] }}", {{ struct[1][1] }});
        endStruct();
    {% endfor %}

    lua_pop(L, 2);
}
