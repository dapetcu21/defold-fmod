#include <string.h>
#include "fmod_bridge.hpp"
#include <LuaBridge/LuaBridge.h>

void * FMODBridge_pushStruct(lua_State *L, const void * structData, size_t structSize, int registryIndex) {
    void * userdata = lua_newuserdata(L, structSize);
    if (structData) { memcpy(userdata, structData, structSize); }

    lua_rawgeti(L, LUA_REGISTRYINDEX, registryIndex);
    lua_setmetatable(L, -2);
    return userdata
}

void * FMODBridge_checkStruct(lua_State *L, int index, int registryIndex, const char * structName) {
    void * data = lua_touserdata(L, index);
    lua_getfield(L, LUA_REGISTRYINDEX, registryIndex);
    if (data == NULL || !lua_getmetatable(L, index) || !lua_rawequal(L, -1, -2)) {
        luaL_typerror(L, index, structName);
    }
    lua_pop(L, 2);
    return data;
}

#define declareStructAccessors(structName) \
    int CONCAT(FMODBridge_registry_, structName) = LUA_REFNIL; \
    inline structName * CONCAT(FMODBridge_push_, structName)(lua_State *L, const structName * structData) { \
        return FMODBridge_pushStruct(L, structData, sizeof(structName), CONCAT(FMODBridge_registry_, structName)); \
    } \
    inline structName * CONCAT(FMODBridge_check_, structName)(lua_State *L, int index) { \
        return FMODBridge_checkStruct(L, index, CONCAT(FMODBridge_registry_, structName), STRINGIFY(structName)); \
    }

{% for struct in structs
    %}declareStructAccessors({{ struct[0] }});
{% endfor %}

void FMODBridge_register(lua_State *L) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "fmod");

    #define addEnum(x) \
        lua_pushstring(L, #x); \
        lua_pushnumber(L, (lua_Number)CONCAT(FMOD_, x)); \
        lua_rawset(L, -3)

    {% for enum in enums
        %}addEnum({{ enum }});
    {% endfor %}
}
