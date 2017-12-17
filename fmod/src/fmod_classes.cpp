#include "fmod_bridge.hpp"
#include "fmod_helpers.hpp"
#include <LuaBridge/LuaBridge.h>
#include <string.h>

using namespace luabridge;
using namespace FMODBridge;

namespace FMODBridge {
    class StudioBank {
        FMOD::Studio::Bank* instance;
    public:
        StudioBank(FMOD::Studio::Bank *instance_): instance(instance_) {}
        stringGetter(instance, getPath)
    };

    class StudioEventDescription {
        FMOD::Studio::EventDescription* instance;
    public:
        StudioEventDescription(FMOD::Studio::EventDescription *instance_): instance(instance_) {};
        stringGetter(instance, getPath)
    };

    namespace StudioSystem {
        StudioBank loadBankMemory(dmScript::LuaHBuffer* buffer, FMOD_STUDIO_LOAD_BANK_FLAGS flags, lua_State* L) {
            FMOD::Studio::Bank *result;

            uint32_t size;
            void* bytes;
            if (dmBuffer::GetBytes(buffer->m_Buffer, &bytes, &size) != dmBuffer::RESULT_OK) {
                lua_pushstring(L, "dmBuffer::GetBytes failed");
                lua_error(L);
            }

            errCheck(system->loadBankMemory((char*)bytes, size, FMOD_STUDIO_LOAD_MEMORY, flags, &result));
            return result;
        }

        StudioEventDescription getEvent(const char* path, lua_State* L) {
            FMOD::Studio::EventDescription *result;
            errCheck(system->getEvent(path, &result));
            return result;
        }
    };

};

void FMODBridge::registerClasses(lua_State *L) {
    getGlobalNamespace(L)
        .beginNamespace("fmod")
            .beginClass<FMOD_GUID>("Guid")
            .endClass()
            .beginNamespace("system")
            .endNamespace()
            .beginNamespace("studio")
                .beginClass<StudioBank>("Bank")
                    .addFunction("get_path", &StudioBank::getPath)
                .endClass()
                .beginClass<StudioEventDescription>("EventDescription")
                    .addFunction("get_path", &StudioEventDescription::getPath)
                .endClass()
                .beginNamespace("system")
                    .addFunction("load_bank_memory", &StudioSystem::loadBankMemory)
                    .addFunction("get_event", &StudioSystem::getEvent)
                .endNamespace()
            .endNamespace()
        .endNamespace();
}
