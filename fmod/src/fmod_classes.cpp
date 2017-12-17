#include "fmod_bridge.hpp"
#include "fmod_helpers.hpp"
#include <LuaBridge/LuaBridge.h>
#include <string>
#include <map>

using namespace luabridge;
using namespace FMODBridge;

std::map<void*, int> FMODBridge::refCounts;

namespace FMODBridge {
    class StudioEventDescription;

    class StudioBank {
        FMOD::Studio::Bank* instance;
    public:
        StudioBank(FMOD::Studio::Bank *instance_): instance(instance_) {}
        makeStringGetter(instance, getPath)
    };

    class StudioEventInstance: public RefCountedProxy<FMOD::Studio::EventInstance> {
    public:
        StudioEventInstance(FMOD::Studio::EventInstance* instance): RefCountedProxy(instance) {}
        makeMethod(instance, start)
        makeMethod1(instance, stop, FMOD_STUDIO_STOP_MODE)
        makeValueGetter(FMOD_3D_ATTRIBUTES, instance, get3DAttributes)
        makeMethod1(instance, set3DAttributes, const FMOD_3D_ATTRIBUTES*)
        StudioEventDescription getDescription(lua_State* L);
        makeMethod2(instance, setParameterValue, const char*, float);
    };

    class StudioEventDescription {
        FMOD::Studio::EventDescription* instance;
    public:
        StudioEventDescription(FMOD::Studio::EventDescription *instance_): instance(instance_) {};
        makeStringGetter(instance, getPath)
        makeCastGetter(StudioEventInstance, FMOD::Studio::EventInstance*, instance, createInstance)
    };

    // TODO: Allow methods that need forward declarations to be generated with a macro as well
    StudioEventDescription StudioEventInstance::getDescription(lua_State* L) {
        FMOD::Studio::EventDescription* result;
        errCheck(instance->getDescription(&result));
        return result;
    }

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
                .beginClass<StudioEventInstance>("EventInstance")
                    .addFunction("start", &StudioEventInstance::start)
                    .addFunction("stop", &StudioEventInstance::stop)
                    .addFunction("get_description", &StudioEventInstance::getDescription)
                    .addFunction("get_3d_attributes", &StudioEventInstance::get3DAttributes)
                    .addFunction("set_3d_attributes", &StudioEventInstance::set3DAttributes)
                    .addFunction("set_parameter_value", &StudioEventInstance::setParameterValue)
                .endClass()
                .beginClass<StudioEventDescription>("EventDescription")
                    .addFunction("create_instance", &StudioEventDescription::createInstance)
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
