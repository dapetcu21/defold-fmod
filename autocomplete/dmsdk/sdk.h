// This header exists so that clang_complete doesn't crash when it doesn't find dmsdk/sdk.h
#ifndef DMSDK_H
#define DMSDK_H

#include <stdint.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

namespace dmBuffer {
    struct Buffer;
    typedef struct Buffer* HBuffer;

    enum Result {
        RESULT_OK,
    };

    Result GetBytes(HBuffer*, void**, uint32_t*);
};

namespace dmScript {
    struct LuaHBuffer {
        dmBuffer::HBuffer* m_Buffer;
        bool m_Ownership;
    };

    LuaHBuffer* CheckBuffer(lua_State *, int);
    void PushBuffer(lua_State *, LuaHBuffer);
};

#endif
