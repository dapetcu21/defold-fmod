#include <string.h>
#include <stddef.h>
#include "fmod_bridge.hpp"
#include "fmod_errors.h"

inline static void errCheck_(FMOD_RESULT res, lua_State* L) {
    if (res != FMOD_OK) {
        lua_pushstring(L, FMOD_ErrorString(res));
        lua_error(L);
    }
}
#define errCheck(res) errCheck_(res, L)

#define FMODBridge_push_char(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_char(L, index) ((char)luaL_checknumber(L, index))
#define FMODBridge_push_short(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_short(L, index) ((short)luaL_checknumber(L, index))
#define FMODBridge_push_int(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_int(L, index) ((int)luaL_checknumber(L, index))
#define FMODBridge_push_long(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_long(L, index) ((long)luaL_checknumber(L, index))
#define FMODBridge_push_unsigned_char(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_unsigned_char(L, index) ((unsigned char)luaL_checknumber(L, index))
#define FMODBridge_push_unsigned_short(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_unsigned_short(L, index) ((unsigned short)luaL_checknumber(L, index))
#define FMODBridge_push_unsigned_int(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_unsigned_int(L, index) ((unsigned int)luaL_checknumber(L, index))
#define FMODBridge_push_unsigned_long(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_unsigned_long(L, index) ((unsigned long)luaL_checknumber(L, index))
#define FMODBridge_push_float(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_float(L, index) ((float)luaL_checknumber(L, index))
#define FMODBridge_push_double(L, x) lua_pushnumber(L, (lua_Number)(x))
#define FMODBridge_check_double(L, index) ((double)luaL_checknumber(L, index))
#define FMODBridge_push_FMOD_BOOL(L, x) lua_pushboolean(L, (x))
#define FMODBridge_check_FMOD_BOOL(L, index) (luaL_checktype(L, index, LUA_TBOOLEAN), (FMOD_BOOL)lua_toboolean(L, index))

#define FMODBridge_push_ptr_char _FMODBridge_push_ptr_char
inline void _FMODBridge_push_ptr_char(lua_State *L, const char * x) {
    if (x) {
        lua_pushstring(L, x);
    } else {
        lua_pushnil(L);
    }
}
#define FMODBridge_check_ptr_char(L, index) luaL_checkstring(L, index)

#define FMODBridge_check_FMOD_VECTOR FMODBridge_dmScript_CheckVector3
#define FMODBridge_push_FMOD_VECTOR _FMODBridge_push_FMOD_VECTOR
inline void _FMODBridge_push_FMOD_VECTOR(lua_State *L, FMOD_VECTOR vec) {
    FMODBridge_dmScript_PushVector3(L, vec.x, vec.y, vec.z);
}
#define FMODBridge_push_ptr_FMOD_VECTOR _FMODBridge_push_ptr_FMOD_VECTOR
inline void _FMODBridge_push_ptr_FMOD_VECTOR(lua_State *L, const FMOD_VECTOR * vec) {
    FMODBridge_dmScript_PushVector3(L, vec->x, vec->y, vec->z);
}

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

static void * pushClass(lua_State *L, void * instance, int registryIndex) {
    void ** userdata = (void**)lua_newuserdata(L, sizeof(void*));
    *userdata = instance;

    lua_rawgeti(L, LUA_REGISTRYINDEX, registryIndex);
    lua_setmetatable(L, -2);
    return instance;
}

#define checkClass(L, index, registryIndex, structName) (*((void**)checkStruct(L, index, registryIndex, structName)))

{% for struct in structs
    %}static int FMODBridge_registry_{{ struct.name }} = LUA_REFNIL;
{% endfor %}

#define declarePropertyGetter(typename, type) \
    static int CONCAT(FMODBridge_propertyGet_, typename)(lua_State *L) { \
        char *base = (char*)lua_touserdata(L, 1); \
        size_t offset = (size_t)lua_touserdata(L, lua_upvalueindex(1)); \
        type *member = (type*)(base + offset); \
        CONCAT(FMODBridge_push_, typename)(L, *member); \
        return 1; \
    }

#define declarePropertyGetterPtr(typename, type) \
    static int CONCAT(FMODBridge_propertyGet_, typename)(lua_State *L) { \
        char *base = (char*)lua_touserdata(L, 1); \
        size_t offset = (size_t)lua_touserdata(L, lua_upvalueindex(1)); \
        type *member = (type*)(base + offset); \
        CONCAT(FMODBridge_push_ptr_, typename)(L, member); \
        return 1; \
    }

#define declarePropertySetter(typename, type) \
    static int CONCAT(FMODBridge_propertySet_, typename)(lua_State *L) { \
        char *base = (char*)lua_touserdata(L, 1); \
        size_t offset = (size_t)lua_touserdata(L, lua_upvalueindex(1)); \
        type *member = (type*)(base + offset); \
        *member = CONCAT(FMODBridge_check_, typename)(L, 2); \
        return 0; \
    } \

#define declarePropertySetterPtr(typename, type) \
    static int CONCAT(FMODBridge_propertySet_, typename)(lua_State *L) { \
        char *base = (char*)lua_touserdata(L, 1); \
        size_t offset = (size_t)lua_touserdata(L, lua_upvalueindex(1)); \
        type *member = (type*)(base + offset); \
        *member = *CONCAT(FMODBridge_check_ptr_, typename)(L, 2); \
        return 0; \
    } \

declarePropertyGetter(char, char);
declarePropertySetter(char, char);
declarePropertyGetter(short, short);
declarePropertySetter(short, short);
declarePropertyGetter(int, int);
declarePropertySetter(int, int);
declarePropertyGetter(long, long);
declarePropertySetter(long, long);
declarePropertyGetter(unsigned_char, unsigned char);
declarePropertySetter(unsigned_char, unsigned char);
declarePropertyGetter(unsigned_short, unsigned short);
declarePropertySetter(unsigned_short, unsigned short);
declarePropertyGetter(unsigned_int, unsigned int);
declarePropertySetter(unsigned_int, unsigned int);
declarePropertyGetter(unsigned_long, unsigned long);
declarePropertySetter(unsigned_long, unsigned long);
declarePropertyGetter(float, float);
declarePropertySetter(float, float);
declarePropertyGetter(double, double);
declarePropertySetter(double, double);
declarePropertyGetter(FMOD_BOOL, FMOD_BOOL);
declarePropertySetter(FMOD_BOOL, FMOD_BOOL);
declarePropertyGetter(ptr_char, char*);
declarePropertyGetterPtr(FMOD_VECTOR, FMOD_VECTOR);
declarePropertySetter(FMOD_VECTOR, FMOD_VECTOR);

{% for struct in structs %}
    #ifndef FMODBridge_push_ptr_{{ struct.name }}{% if struct.is_class %}
    #define FMODBridge_push_ptr_{{ struct.name }}(L, instance) (({{ struct.name }}*)pushClass(L, instance, FMODBridge_registry_{{ struct.name }}))
    {% else %}
    #define FMODBridge_push_ptr_{{ struct.name }}(L, structData) (({{ struct.name }}*)pushStruct(L, structData, sizeof({{ struct.name }}), FMODBridge_registry_{{ struct.name }}))
    {% endif %}
    #endif
    #ifndef FMODBridge_check_ptr_{{ struct.name }}{% if struct.is_class %}
    #define FMODBridge_check_ptr_{{ struct.name }}(L, index) (({{ struct.name }}*)checkClass(L, index, FMODBridge_registry_{{ struct.name }}, "{{ struct.name }}"))
    {% else %}
    #define FMODBridge_check_ptr_{{ struct.name }}(L, index) (({{ struct.name }}*)checkStruct(L, index, FMODBridge_registry_{{ struct.name }}, "{{ struct.name }}"))
    {% endif %}
    #endif
    #ifdef FMODBridge_propertyOverride_{{ struct.name }}
    FMODBridge_propertyOverride_{{ struct.name }}
    #else
    declarePropertyGetter(ptr_{{ struct.name }}, {{ struct.name }}*);{% if not struct.is_class %}
    declarePropertyGetterPtr({{ struct.name }}, {{ struct.name }});
    declarePropertySetterPtr({{ struct.name }}, {{ struct.name }});
    {% endif %}
    #endif
{% endfor %}

static int structConstructor(lua_State *L) {
    int registryIndex = (int)lua_tonumber(L, lua_upvalueindex(1));
    size_t structSize = (size_t)lua_tonumber(L, lua_upvalueindex(2));
    pushStruct(L, NULL, structSize, registryIndex);
    return 1;
}

static int structIndexMetamethod(lua_State *L) {
    int metatableIndex = lua_upvalueindex(1);

    lua_pushvalue(L, 2);
    lua_rawget(L, metatableIndex);

    if (!lua_isnil(L, -1)) { return 1; }

    lua_pushstring(L, "__fieldget");
    lua_rawget(L, metatableIndex);

    lua_pushvalue(L, 2);
    lua_rawget(L, -2);

    if (!lua_isfunction(L, -1)) { return 1; }

    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);
    return 1;
}

static int structNewIndexMetamethod(lua_State *L) {
    int metatableIndex = lua_upvalueindex(1);

    lua_pushstring(L, "__fieldset");
    lua_rawget(L, metatableIndex);

    lua_pushvalue(L, 2);
    lua_rawget(L, -2);

    if (!lua_isfunction(L, -1)) {
        lua_getglobal(L, "tostring");
        lua_pushvalue(L, 2);
        if (lua_pcall(L, 1, 1, 0) == 0) {
            const char *key = lua_tostring(L, -1);
            luaL_error(L, "Cannot set invalid property \"%s\"", key);
        } else {
            luaL_error(L, "Cannot set invalid property");
        }
    }

    lua_pushvalue(L, 1);
    lua_pushvalue(L, 3);
    lua_call(L, 2, 0);
    return 0;
}

void * FMODBridge_check_ptr_void_size(lua_State *L, int index, size_t *length) {
    if (lua_type(L, index) == LUA_TSTRING) {
        return (char*)lua_tolstring(L, index, length);
    }
    FMODBridge_HBuffer buffer = FMODBridge_dmScript_CheckBuffer(L, index);
    void *data;
    uint32_t size;
    if (FMODBridge_dmBuffer_GetBytes(buffer, &data, &size)) {
        luaL_error(L, "dmBuffer::GetBytes failed");
    }
    *length = size;
    return data;
}

#define FMODBridge_func_FMOD_Studio_System_LoadBankMemory _FMODBridge_func_FMOD_Studio_System_LoadBankMemory
static int _FMODBridge_func_FMOD_Studio_System_LoadBankMemory(lua_State *L) {
    FMOD_STUDIO_SYSTEM *system = FMODBridge_check_ptr_FMOD_STUDIO_SYSTEM(L, 1);
    size_t length;
    const char *buffer = (const char*)FMODBridge_check_ptr_void_size(L, 2, &length);
    FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMODBridge_check_unsigned_int(L, 3);

    FMOD_STUDIO_BANK *bank;

    ensure(ST, FMOD_Studio_System_LoadBankMemory, FMOD_RESULT, FMOD_STUDIO_SYSTEM *system, const char *buffer, int length, FMOD_STUDIO_LOAD_MEMORY_MODE mode, FMOD_STUDIO_LOAD_BANK_FLAGS flags, FMOD_STUDIO_BANK **bank);
    errCheck(FMOD_Studio_System_LoadBankMemory(system, buffer, (int)length, FMOD_STUDIO_LOAD_MEMORY, flags, &bank));

    FMODBridge_push_ptr_FMOD_STUDIO_BANK(L, bank);
    return 1;
}


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
        CONCAT(FMODBridge_registry_, structName) = luaL_ref(L, LUA_REGISTRYINDEX); \
        \
        lua_pushvalue(L, -1); \
        lua_pushcclosure(L, &structIndexMetamethod, 1); \
        lua_setfield(L, -2, "__index"); \
        \
        lua_pushvalue(L, -1); \
        lua_pushcclosure(L, &structNewIndexMetamethod, 1); \
        lua_setfield(L, -2, "__newindex"); \
        \
        lua_newtable(L); \
        lua_pushvalue(L, -1); \
        lua_setfield(L, -3, "__fieldget"); \
        \
        lua_newtable(L); \
        lua_pushvalue(L, -1); \
        lua_setfield(L, -4, "__fieldset")

    #define endStruct() lua_pop(L, 3)

    #define addStructConstructor(structName, name, containerIndex) \
        lua_pushnumber(L, CONCAT(FMODBridge_registry_, structName)); \
        lua_pushnumber(L, sizeof(structName)); \
        lua_pushcclosure(L, &structConstructor, 2); \
        lua_setfield(L, containerIndex - 4, name)

    #define addPropertyGetter(structName, name, typename) \
        lua_pushlightuserdata(L, (void*)offsetof(structName, name)); \
        lua_pushcclosure(L, &CONCAT(FMODBridge_propertyGet_, typename), 1); \
        lua_setfield(L, -3, STRINGIFY(name))

    #define addPropertySetter(structName, name, typename) \
        lua_pushlightuserdata(L, (void*)offsetof(structName, name)); \
        lua_pushcclosure(L, &CONCAT(FMODBridge_propertySet_, typename), 1); \
        lua_setfield(L, -2, STRINGIFY(name))

    #define addProperty(structName, name, typename) \
        addPropertyGetter(structName, name, typename); \
        addPropertySetter(structName, name, typename)

    {% for struct in structs %}beginStruct({{ struct.name }});
        {% if not struct.is_class %}addStructConstructor({{ struct.name }}, "{{ struct.constructor_name }}", {{ struct.constructor_table }});
        {% endif %}{% for property in struct.properties %}/* {{ property[1].c_type }} {{ property[0] }} */
        {% if property[1].readable %}addPropertyGetter({{ struct.name }}, {{ property[0] }}, {{ property[1].name }});
        {% endif %}{% if property[1].writeable %}addPropertySetter({{ struct.name }}, {{ property[0] }}, {{ property[1].name }});
        {% endif %}{% endfor %}{% for f in struct.methods %}#ifdef FMODBridge_func_{{ f[1].name }}
        lua_pushcfunction(L, &FMODBridge_func_{{ f[1].name }});
        lua_setfield(L, -4, "{{ f[0] }}");
        #endif
        {% endfor %}#ifdef FMODBridge_extras_{{ struct.name }}
        FMODBridge_extras_{{ struct.name }}
        #endif
    endStruct();
    {% endfor %}

    {% for f in global_functions %}
        #ifdef FMODBridge_func_{{ f[2].name }}
        lua_pushcfunction(L, &FMODBridge_func_{{ f[2].name }});
        lua_setfield(L, {{ f[0] }} - 1, "{{ f[1] }}");
        #endif
    {% endfor %}

    FMODBridge_push_ptr_FMOD_STUDIO_SYSTEM(L, FMODBridge_system);
    lua_setfield(L, -2, "system");

    FMODBridge_push_ptr_FMOD_SYSTEM(L, FMODBridge_lowLevelSystem);
    lua_setfield(L, -3, "system");

    lua_pop(L, 2);
}
