#include "fmod_bridge.hpp"
#include "fmod_helpers.hpp"
#include <LuaBridge/LuaBridge.h>
#include <string>
#include <map>

using namespace luabridge;
using namespace FMODBridge;

std::map<void*, int> FMODBridge::refCounts;

namespace FMODBridge {
    // LOWLEVEL CLASSES

    class ChannelGroup: public Proxy<FMOD::ChannelGroup> {
    public:
        ChannelGroup(FMOD::ChannelGroup *instance): Proxy(instance) {}
    };


    class System: public Proxy<FMOD::System> {
    public:
        System(FMOD::System* instance): Proxy(instance) {}
    };

    // STUDIO CLASSES

    class StudioEventDescription;

    class StudioVCA: public Proxy<FMOD::Studio::VCA> {
    public:
        StudioVCA(FMOD::Studio::VCA *instance): Proxy(instance) {}
        makeGetter(getID, FMOD_GUID);
        makeStringGetter(getPath);
        make2FloatGetter(getVolume);
        makeMethod1(setVolume, float);
    };

    class StudioBus: public Proxy<FMOD::Studio::Bus> {
    public:
        StudioBus(FMOD::Studio::Bus *instance): Proxy(instance) {}
        makeGetter(getID, FMOD_GUID);
        makeStringGetter(getPath);
    };

    class StudioBank: public Proxy<FMOD::Studio::Bank> {
    public:
        StudioBank(FMOD::Studio::Bank *instance): Proxy(instance) {}
        makeGetter(getID, FMOD_GUID);
        makeStringGetter(getPath);
    };

    class StudioEventInstance: public RefCountedProxy<FMOD::Studio::EventInstance> {
    public:
        StudioEventInstance(FMOD::Studio::EventInstance* instance): RefCountedProxy(instance) {}
        makeGetter(get3DAttributes, FMOD_3D_ATTRIBUTES);
        makeCastGetter(getChannelGroup, ChannelGroup, FMOD::ChannelGroup*);
        StudioEventDescription getDescription(lua_State* L);
        makeGetter(getListenerMask, unsigned int);
        makeGetter(getParameterCount, int);
        make2FloatGetter1(getParameterValue, const char*);
        make2FloatGetter1(getParameterValueByIndex, int);
        makeGetter(getPaused, bool);
        make2FloatGetter(getPitch);
        makeGetter(getPlaybackState, FMOD_STUDIO_PLAYBACK_STATE);
        makeGetter1(getProperty, float, FMOD_STUDIO_EVENT_PROPERTY);
        makeGetter1(getReverbLevel, float, int);
        makeGetter(getTimelinePosition, int);
        make2FloatGetter(getVolume);
        makeGetter(isVirtual, bool);
        makeMethod1(set3DAttributes, const FMOD_3D_ATTRIBUTES*);
        makeMethod1(setListenerMask, unsigned int);
        makeMethod2(setParameterValue, const char*, float);
        makeMethod2(setParameterValueByIndex, int, float);
        makeMethod1(setPaused, bool);
        makeMethod1(setPitch, float);
        makeMethod2(setProperty, FMOD_STUDIO_EVENT_PROPERTY, float);
        makeMethod2(setReverbLevel, int, float);
        makeMethod1(setTimelinePosition, int);
        makeMethod1(setVolume, float);
        makeMethod(start);
        makeMethod1(stop, FMOD_STUDIO_STOP_MODE);
        makeMethod(triggerCue);
    };

    class StudioEventDescription: public Proxy<FMOD::Studio::EventDescription> {
    public:
        StudioEventDescription(FMOD::Studio::EventDescription *instance): Proxy(instance) {};
        makeStringGetter(getPath);
        makeCastGetter(createInstance, StudioEventInstance, FMOD::Studio::EventInstance*);
    };

    defineCastGetter(StudioEventInstance::getDescription, getDescription, StudioEventDescription, FMOD::Studio::EventDescription*);

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

        makeCastGetter1(getBank, StudioBank, FMOD::Studio::Bank*, const char*);
        makeCastGetter1(getBankByID, StudioBank, FMOD::Studio::Bank*, const FMOD_GUID*);
        makeCastGetter1(getBus, StudioBus, FMOD::Studio::Bus*, const char*);
        makeCastGetter1(getBusByID, StudioBus, FMOD::Studio::Bus*, const FMOD_GUID*);
        makeCastGetter1(getEvent, StudioEventDescription, FMOD::Studio::EventDescription*, const char*);
        makeCastGetter1(getEventByID, StudioEventDescription, FMOD::Studio::EventDescription*, const FMOD_GUID*);
        makeGetter1(getListenerAttributes, FMOD_3D_ATTRIBUTES, int);
        makeCastGetter1(getVCA, StudioVCA, FMOD::Studio::VCA*, const char*);
        makeCastGetter1(getVCAByID, StudioVCA, FMOD::Studio::VCA*, const FMOD_GUID*);
        makeGetter1(lookupID, FMOD_GUID, const char*);
        makeStringGetter1(lookupPath, const FMOD_GUID*);
        makeMethod2(setListenerAttributes, int, const FMOD_3D_ATTRIBUTES*);
    };

    FMOD_GUID parseID(const char *idString, lua_State* L) {
        FMOD_GUID id;
        errCheck(FMOD::Studio::parseID(idString, &id));
        return id;
    }
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
            // Low-Level bindings
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

            // Studio bindings
            .beginNamespace("studio")
                .addFunction("parse_id", &parseID)
                .beginClass<StudioBus>("Bus")
                    .addFunction("get_id", &StudioBus::getID)
                    .addFunction("get_path", &StudioBus::getPath)
                .endClass()
                .beginClass<StudioBank>("Bank")
                    .addFunction("get_id", &StudioBank::getID)
                    .addFunction("get_path", &StudioBank::getPath)
                .endClass()
                .beginClass<StudioEventInstance>("EventInstance")
                    .addFunction("get_3d_attributes", &StudioEventInstance::get3DAttributes)
                    .addFunction("get_channel_group", &StudioEventInstance::getChannelGroup)
                    .addFunction("get_description", &StudioEventInstance::getDescription)
                    .addFunction("get_listener_mask", &StudioEventInstance::getListenerMask)
                    .addFunction("get_parameter_count", &StudioEventInstance::getParameterCount)
                    .addCFunction("get_parameter_value", &StudioEventInstance::getParameterValue)
                    .addCFunction("get_parameter_value_by_index", &StudioEventInstance::getParameterValueByIndex)
                    .addFunction("get_paused", &StudioEventInstance::getPaused)
                    .addCFunction("get_pitch", &StudioEventInstance::getPitch)
                    .addFunction("get_playback_state", &StudioEventInstance::getPlaybackState)
                    .addFunction("get_property", &StudioEventInstance::getProperty)
                    .addFunction("get_reverb_level", &StudioEventInstance::getReverbLevel)
                    .addFunction("get_timeline_position", &StudioEventInstance::getTimelinePosition)
                    .addCFunction("get_volume", &StudioEventInstance::getVolume)
                    .addFunction("is_virtual", &StudioEventInstance::isVirtual)
                    .addFunction("set_3d_attributes", &StudioEventInstance::set3DAttributes)
                    .addFunction("set_listener_mask", &StudioEventInstance::setListenerMask)
                    .addFunction("set_parameter_value", &StudioEventInstance::setParameterValue)
                    .addFunction("set_parameter_value_by_index", &StudioEventInstance::setParameterValueByIndex)
                    .addFunction("set_paused", &StudioEventInstance::setPaused)
                    .addFunction("set_pitch", &StudioEventInstance::setPitch)
                    .addFunction("set_property", &StudioEventInstance::setProperty)
                    .addFunction("set_reverb_level", &StudioEventInstance::setReverbLevel)
                    .addFunction("set_timeline_position", &StudioEventInstance::setTimelinePosition)
                    .addFunction("set_volume", &StudioEventInstance::setVolume)
                    .addFunction("start", &StudioEventInstance::start)
                    .addFunction("stop", &StudioEventInstance::stop)
                    .addFunction("trigger_cue", &StudioEventInstance::triggerCue)
                .endClass()
                .beginClass<StudioEventDescription>("EventDescription")
                    .addFunction("create_instance", &StudioEventDescription::createInstance)
                    .addFunction("get_path", &StudioEventDescription::getPath)
                .endClass()
                .beginClass<StudioVCA>("VCA")
                    .addFunction("get_id", &StudioVCA::getID)
                    .addFunction("get_path", &StudioVCA::getPath)
                    .addCFunction("get_volume", &StudioVCA::getVolume)
                    .addFunction("set_volume", &StudioVCA::setVolume)
                .endClass()
                .beginClass<StudioSystem>("System")
                    .addFunction("get_bank", &StudioSystem::getBank)
                    .addFunction("get_bank_by_id", &StudioSystem::getBankByID)
                    .addFunction("get_bus", &StudioSystem::getBus)
                    .addFunction("get_bus_by_id", &StudioSystem::getBusByID)
                    .addFunction("get_event", &StudioSystem::getEvent)
                    .addFunction("get_event_by_id", &StudioSystem::getEventByID)
                    .addFunction("get_listener_attributes", &StudioSystem::getListenerAttributes)
                    .addFunction("get_vca", &StudioSystem::getVCA)
                    .addFunction("get_vca_by_id", &StudioSystem::getVCAByID)
                    .addFunction("load_bank_memory", &StudioSystem::loadBankMemory)
                    .addFunction("lookup_id", &StudioSystem::lookupID)
                    .addFunction("lookup_path", &StudioSystem::lookupPath)
                    .addFunction("set_listener_attributes", &StudioSystem::setListenerAttributes)
                .endClass()
                .addVariable("system", &FMODBridge::system, false)
            .endNamespace()
        .endNamespace();
}
