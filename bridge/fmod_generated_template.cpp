#include "fmod_bridge.hpp"
#include <LuaBridge/LuaBridge.h>

void FMODBridge_register(lua_State *L) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "fmod");

    #define add_enum(x) \
        lua_pushstring(L, #x); \
        lua_pushnumber(L, (lua_Number)CONCAT(FMOD_, x)); \
        lua_rawset(L, -3)

    {% for enum in enums
        %}add_enum({{ enum }})
    {% endfor %}
}
