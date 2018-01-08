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
    #define currentLib LL

    class ChannelGroup: public Proxy<FMOD_CHANNELGROUP> {
    public:
        ChannelGroup(FMOD_CHANNELGROUP *instance): Proxy(instance) {}
    };


    class System: public Proxy<FMOD_SYSTEM> {
    public:
        System(FMOD_SYSTEM* instance): Proxy(instance) {}
    };

    // STUDIO CLASSES
    #undef currentLib
    #define currentLib ST

    class StudioEventDescription;

    class StudioVCA: public Proxy<FMOD_STUDIO_VCA> {
    #define currentClass VCA
    public:
        StudioVCA(FMOD_STUDIO_VCA *instance): Proxy(instance) {}
        makeGetter(GetID, FMOD_GUID);
        makeStringGetter(GetPath);
        make2FloatGetter(GetVolume);
        makeMethod1(SetVolume, float);
    #undef currentClass
    };

    class StudioBus: public Proxy<FMOD_STUDIO_BUS> {
    #define currentClass Bus
    public:
        StudioBus(FMOD_STUDIO_BUS *instance): Proxy(instance) {}
        makeGetter(GetID, FMOD_GUID);
        makeStringGetter(GetPath);
    #undef currentClass
    };

    class StudioBank: public Proxy<FMOD_STUDIO_BANK> {
    #define currentClass Bank
    public:
        StudioBank(FMOD_STUDIO_BANK *instance): Proxy(instance) {}
        makeGetter(GetID, FMOD_GUID);
        makeStringGetter(GetPath);
    #undef currentClass
    };

    class StudioEventInstance: public RefCountedProxy<FMOD_STUDIO_EVENTINSTANCE> {
    #define currentClass EventInstance
    public:
        makeProxyConstructor(StudioEventInstance, FMOD_STUDIO_EVENTINSTANCE);
        makeGetter(Get3DAttributes, FMOD_3D_ATTRIBUTES);
        makeCastGetter(GetChannelGroup, ChannelGroup, FMOD_CHANNELGROUP*);
        StudioEventDescription GetDescription(lua_State* L);
        makeGetter(GetListenerMask, unsigned int);
        makeGetter(GetParameterCount, int);
        make2FloatGetter1(GetParameterValue, const char*);
        make2FloatGetter1(GetParameterValueByIndex, int);
        makeCastGetter(GetPaused, bool, FMOD_BOOL);
        make2FloatGetter(GetPitch);
        makeGetter(GetPlaybackState, FMOD_STUDIO_PLAYBACK_STATE);
        makeGetter1(GetProperty, float, FMOD_STUDIO_EVENT_PROPERTY);
        makeGetter1(GetReverbLevel, float, int);
        makeGetter(GetTimelinePosition, int);
        make2FloatGetter(GetVolume);
        makeCastGetter(IsVirtual, bool, FMOD_BOOL);
        makeMethod1(Set3DAttributes, FMOD_3D_ATTRIBUTES*);
        makeMethod1(SetListenerMask, unsigned int);
        makeMethod2(SetParameterValue, const char*, float);
        makeMethod2(SetParameterValueByIndex, int, float);
        makeMethod1(SetPaused, bool);
        makeMethod1(SetPitch, float);
        makeMethod2(SetProperty, FMOD_STUDIO_EVENT_PROPERTY, float);
        makeMethod2(SetReverbLevel, int, float);
        makeMethod1(SetTimelinePosition, int);
        makeMethod1(SetVolume, float);
        makeMethod(Start);
        makeMethod1(Stop, FMOD_STUDIO_STOP_MODE);
        makeMethod(TriggerCue);
    #undef currentClass
    };

    class StudioEventDescription: public Proxy<FMOD_STUDIO_EVENTDESCRIPTION> {
    #define currentClass EventDescription
    public:
        StudioEventDescription(FMOD_STUDIO_EVENTDESCRIPTION *instance): Proxy(instance) {};
        makeStringGetter(GetPath);
        makeCastGetter(CreateInstance, StudioEventInstance, FMOD_STUDIO_EVENTINSTANCE*);
    #undef currentClass
    };

    #define currentClass EventInstance
    defineCastGetter(StudioEventInstance::GetDescription, GetDescription, StudioEventDescription, FMOD_STUDIO_EVENTDESCRIPTION*);
    #undef currentClass

    class StudioSystem: public Proxy<FMOD_STUDIO_SYSTEM> {
    #define currentClass System
    public:
        StudioSystem(FMOD_STUDIO_SYSTEM* instance): Proxy(instance) {}

        StudioBank LoadBankMemory(LuaHBuffer* buffer, FMOD_STUDIO_LOAD_BANK_FLAGS flags, lua_State* L) {
            FMOD_STUDIO_BANK *result;

            uint32_t size;
            void* bytes;
            if (FMODBridge_dmBuffer_GetBytes(buffer->handle, &bytes, &size) != 0) {
                lua_pushstring(L, "dmBuffer::GetBytes failed");
                lua_error(L);
            }

            errCheck(FMOD_Studio_System_LoadBankMemory(instance,
                (char*)bytes, size, FMOD_STUDIO_LOAD_MEMORY, flags, &result
            ));
            return result;
        }

        makeCastGetter1(GetBank, StudioBank, FMOD_STUDIO_BANK*, const char*);
        makeCastGetter1(GetBankByID, StudioBank, FMOD_STUDIO_BANK*, const FMOD_GUID*);
        makeCastGetter1(GetBus, StudioBus, FMOD_STUDIO_BUS*, const char*);
        makeCastGetter1(GetBusByID, StudioBus, FMOD_STUDIO_BUS*, const FMOD_GUID*);
        makeCastGetter1(GetEvent, StudioEventDescription, FMOD_STUDIO_EVENTDESCRIPTION*, const char*);
        makeCastGetter1(GetEventByID, StudioEventDescription, FMOD_STUDIO_EVENTDESCRIPTION*, const FMOD_GUID*);
        makeGetter1(GetListenerAttributes, FMOD_3D_ATTRIBUTES, int);
        makeCastGetter1(GetVCA, StudioVCA, FMOD_STUDIO_VCA*, const char*);
        makeCastGetter1(GetVCAByID, StudioVCA, FMOD_STUDIO_VCA*, const FMOD_GUID*);
        makeGetter1(LookupID, FMOD_GUID, const char*);
        makeStringGetter1(LookupPath, const FMOD_GUID*);
        makeMethod2(SetListenerAttributes, int, FMOD_3D_ATTRIBUTES*);
    #undef currentClass
    };

    FMOD_GUID parseID(const char *idString, lua_State* L) {
        FMOD_GUID id;
        errCheck(FMOD_Studio_ParseID(idString, &id));
        return id;
    }
};

// Perform the explicit cast to a wrapped instance
namespace luabridge {
    template <>
    struct Stack <FMOD_STUDIO_SYSTEM*> {
      static void push (lua_State* L, FMOD_STUDIO_SYSTEM* instance) {
          StudioSystem wrappedInstance(instance);
          return Stack<StudioSystem>::push(L, wrappedInstance);
      }

      static FMOD_STUDIO_SYSTEM* get(lua_State* L, int index) {
          return Stack<StudioSystem>::get(L, index);
      }
    };

    template <>
    struct Stack <FMOD_SYSTEM*> {
      static void push (lua_State* L, FMOD_SYSTEM* instance) {
          System wrappedInstance(instance);
          return Stack<System>::push(L, wrappedInstance);
      }

      static FMOD_SYSTEM* get(lua_State* L, int index) {
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
                    .addFunction("get_id", &StudioBus::GetID)
                    .addFunction("get_path", &StudioBus::GetPath)
                .endClass()
                .beginClass<StudioBank>("Bank")
                    .addFunction("get_id", &StudioBank::GetID)
                    .addFunction("get_path", &StudioBank::GetPath)
                .endClass()
                .beginClass<StudioEventInstance>("EventInstance")
                    .addFunction("get_3d_attributes", &StudioEventInstance::Get3DAttributes)
                    .addFunction("get_channel_group", &StudioEventInstance::GetChannelGroup)
                    .addFunction("get_description", &StudioEventInstance::GetDescription)
                    .addFunction("get_listener_mask", &StudioEventInstance::GetListenerMask)
                    .addFunction("get_parameter_count", &StudioEventInstance::GetParameterCount)
                    .addCFunction("get_parameter_value", &StudioEventInstance::GetParameterValue)
                    .addCFunction("get_parameter_value_by_index", &StudioEventInstance::GetParameterValueByIndex)
                    .addFunction("get_paused", &StudioEventInstance::GetPaused)
                    .addCFunction("get_pitch", &StudioEventInstance::GetPitch)
                    .addFunction("get_playback_state", &StudioEventInstance::GetPlaybackState)
                    .addFunction("get_property", &StudioEventInstance::GetProperty)
                    .addFunction("get_reverb_level", &StudioEventInstance::GetReverbLevel)
                    .addFunction("get_timeline_position", &StudioEventInstance::GetTimelinePosition)
                    .addCFunction("get_volume", &StudioEventInstance::GetVolume)
                    .addFunction("is_virtual", &StudioEventInstance::IsVirtual)
                    .addFunction("set_3d_attributes", &StudioEventInstance::Set3DAttributes)
                    .addFunction("set_listener_mask", &StudioEventInstance::SetListenerMask)
                    .addFunction("set_parameter_value", &StudioEventInstance::SetParameterValue)
                    .addFunction("set_parameter_value_by_index", &StudioEventInstance::SetParameterValueByIndex)
                    .addFunction("set_paused", &StudioEventInstance::SetPaused)
                    .addFunction("set_pitch", &StudioEventInstance::SetPitch)
                    .addFunction("set_property", &StudioEventInstance::SetProperty)
                    .addFunction("set_reverb_level", &StudioEventInstance::SetReverbLevel)
                    .addFunction("set_timeline_position", &StudioEventInstance::SetTimelinePosition)
                    .addFunction("set_volume", &StudioEventInstance::SetVolume)
                    .addFunction("start", &StudioEventInstance::Start)
                    .addFunction("stop", &StudioEventInstance::Stop)
                    .addFunction("trigger_cue", &StudioEventInstance::TriggerCue)
                .endClass()
                .beginClass<StudioEventDescription>("EventDescription")
                    .addFunction("create_instance", &StudioEventDescription::CreateInstance)
                    .addFunction("get_path", &StudioEventDescription::GetPath)
                .endClass()
                .beginClass<StudioVCA>("VCA")
                    .addFunction("get_id", &StudioVCA::GetID)
                    .addFunction("get_path", &StudioVCA::GetPath)
                    .addCFunction("get_volume", &StudioVCA::GetVolume)
                    .addFunction("set_volume", &StudioVCA::SetVolume)
                .endClass()
                .beginClass<StudioSystem>("System")
                    .addFunction("get_bank", &StudioSystem::GetBank)
                    .addFunction("get_bank_by_id", &StudioSystem::GetBankByID)
                    .addFunction("get_bus", &StudioSystem::GetBus)
                    .addFunction("get_bus_by_id", &StudioSystem::GetBusByID)
                    .addFunction("get_event", &StudioSystem::GetEvent)
                    .addFunction("get_event_by_id", &StudioSystem::GetEventByID)
                    .addFunction("get_listener_attributes", &StudioSystem::GetListenerAttributes)
                    .addFunction("get_vca", &StudioSystem::GetVCA)
                    .addFunction("get_vca_by_id", &StudioSystem::GetVCAByID)
                    .addFunction("load_bank_memory", &StudioSystem::LoadBankMemory)
                    .addFunction("lookup_id", &StudioSystem::LookupID)
                    .addFunction("lookup_path", &StudioSystem::LookupPath)
                    .addFunction("set_listener_attributes", &StudioSystem::SetListenerAttributes)
                .endClass()
                .addVariable("system", &FMODBridge::system, false)
            .endNamespace()
        .endNamespace();
}
