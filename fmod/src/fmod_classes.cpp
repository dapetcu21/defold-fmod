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

    class StudioBank: public Proxy<FMOD::Studio::Bank> {
    public:
        StudioBank(FMOD::Studio::Bank *instance): Proxy(instance) {}
        makeStringGetter(getPath);
    };

    class StudioEventInstance: public RefCountedProxy<FMOD::Studio::EventInstance> {
    public:
        StudioEventInstance(FMOD::Studio::EventInstance* instance): RefCountedProxy(instance) {}
        makeMethod(start);
        makeMethod1(stop, FMOD_STUDIO_STOP_MODE);
        makeGetter(get3DAttributes, FMOD_3D_ATTRIBUTES);
        makeMethod1(set3DAttributes, const FMOD_3D_ATTRIBUTES*);
        StudioEventDescription getDescription(lua_State* L);
        makeMethod2(setParameterValue, const char*, float);
    };

    class StudioEventDescription: public Proxy<FMOD::Studio::EventDescription> {
    public:
        StudioEventDescription(FMOD::Studio::EventDescription *instance): Proxy(instance) {};
        makeStringGetter(getPath);
        makeCastGetter(createInstance, StudioEventInstance, FMOD::Studio::EventInstance*);
    };

    defineCastGetter(StudioEventInstance::getDescription, getDescription, StudioEventDescription, FMOD::Studio::EventDescription*);

    class System: public Proxy<FMOD::System> {
    public:
        System(FMOD::System* instance): Proxy(instance) {}
    };

    class StudioSystem: public Proxy<FMOD::Studio::System> {
    public:
        StudioSystem(FMOD::Studio::System* instance): Proxy(instance) {}

        StudioBank loadBankMemory(dmScript::LuaHBuffer* buffer, FMOD_STUDIO_LOAD_BANK_FLAGS flags, lua_State* L) {
            FMOD::Studio::Bank *result;

            uint32_t size;
            void* bytes;
            if (dmBuffer::GetBytes(buffer->m_Buffer, &bytes, &size) != dmBuffer::RESULT_OK) {
                lua_pushstring(L, "dmBuffer::GetBytes failed");
                lua_error(L);
            }

            errCheck(instance->loadBankMemory((char*)bytes, size, FMOD_STUDIO_LOAD_MEMORY, flags, &result));
            return result;
        }

        makeCastGetter1(getEvent, StudioEventDescription, FMOD::Studio::EventDescription*, const char*);
        makeGetter1(getListenerAttributes, FMOD_3D_ATTRIBUTES, int);
        makeMethod2(setListenerAttributes, int, const FMOD_3D_ATTRIBUTES*);
    };

};

// Perform the explicit cast to a wrapped instance
namespace luabridge {
    template <>
    struct Stack <FMOD::Studio::System*> {
      static void push (lua_State* L, FMOD::Studio::System* instance) {
          StudioSystem wrappedInstance(instance);
          return Stack<StudioSystem>::push(L, wrappedInstance);
      }

      static FMOD::Studio::System* get(lua_State* L, int index) {
          return Stack<StudioSystem>::get(L, index);
      }
    };

    template <>
    struct Stack <FMOD::System*> {
      static void push (lua_State* L, FMOD::System* instance) {
          System wrappedInstance(instance);
          return Stack<System>::push(L, wrappedInstance);
      }

      static FMOD::System* get(lua_State* L, int index) {
          return Stack<System>::get(L, index);
      }
    };
}

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
            .beginClass<System>("System")
            .endClass()
            .addVariable("system", &FMODBridge::lowLevelSystem, false)
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
                .beginClass<StudioSystem>("System")
                    .addFunction("load_bank_memory", &StudioSystem::loadBankMemory)
                    .addFunction("get_event", &StudioSystem::getEvent)
                    .addFunction("get_listener_attributes", &StudioSystem::getListenerAttributes)
                    .addFunction("set_listener_attributes", &StudioSystem::setListenerAttributes)
                .endClass()
                .addVariable("system", &FMODBridge::system, false)
            .endNamespace()
        .endNamespace();
}
