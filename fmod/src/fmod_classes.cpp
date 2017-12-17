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
        makeStringGetter(instance, getPath)
    };

    class StudioEventDescription {
        FMOD::Studio::EventDescription* instance;
    public:
        StudioEventDescription(FMOD::Studio::EventDescription *instance_): instance(instance_) {};
        makeStringGetter(instance, getPath)
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

        makeCastGetter1(StudioEventDescription, FMOD::Studio::EventDescription*, system, getEvent, const char*)
        makeValueGetter1(FMOD_3D_ATTRIBUTES, system, getListenerAttributes, int)
        makeMethod2(system, setListenerAttributes, int, const FMOD_3D_ATTRIBUTES*)
    };

};

void FMODBridge::registerClasses(lua_State *L) {
    getGlobalNamespace(L)
        .beginNamespace("fmod")
            .beginClass<FMOD_GUID>("GUID")
            .endClass()
            .beginClass<FMOD_3D_ATTRIBUTES>("_3D_ATTRIBUTES")
                .addConstructor<void (*)(void)>()
                .addData("position", &FMOD_3D_ATTRIBUTES::position)
                .addData("velocity", &FMOD_3D_ATTRIBUTES::velocity)
                .addData("forward", &FMOD_3D_ATTRIBUTES::forward)
                .addData("up", &FMOD_3D_ATTRIBUTES::up)
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
                    .addFunction("get_listener_attributes", &StudioSystem::getListenerAttributes)
                    .addFunction("set_listener_attributes", &StudioSystem::setListenerAttributes)
                .endNamespace()
            .endNamespace()
        .endNamespace();
}
