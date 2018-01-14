#include "fmod_bridge.hpp"
#include "fmod_helpers.hpp"
#include <LuaBridge/LuaBridge.h>
#include <map>

using namespace luabridge;
using namespace FMODBridge;

std::map<void*, int> FMODBridge::refCounts;

declareEnum(FMOD_STUDIO_EVENT_PROPERTY);
declareEnum(FMOD_STUDIO_INSTANCETYPE);
declareEnum(FMOD_STUDIO_LOADING_STATE);
declareEnum(FMOD_STUDIO_LOAD_MEMORY_MODE);
declareEnum(FMOD_STUDIO_PARAMETER_TYPE);
declareEnum(FMOD_STUDIO_PLAYBACK_STATE);
declareEnum(FMOD_STUDIO_STOP_MODE);
declareEnum(FMOD_STUDIO_USER_PROPERTY_TYPE);

namespace FMODBridge {
    // LOWLEVEL CLASSES
    #define currentLib LL

    class ChannelGroup: public Proxy<FMOD_CHANNELGROUP> {
    #define currentClass ChannelGroup
    #define currentType FMOD_CHANNELGROUP
    public:
        ChannelGroup(FMOD_CHANNELGROUP *instance): Proxy(instance) {}
    #undef currentClass
    #undef currentType
    };


    class System: public Proxy<FMOD_SYSTEM> {
    #define currentClass System
    #define currentType FMOD_SYSTEM
    public:
        System(FMOD_SYSTEM* instance): Proxy(instance) {}
    #undef currentClass
    #undef currentType
    };

    // STUDIO CLASSES
    #undef currentLib
    #define currentLib ST

    class StudioEventDescription;

    class StudioVCA: public Proxy<FMOD_STUDIO_VCA> {
    #define currentClass VCA
    #define currentType FMOD_STUDIO_VCA
    public:
        StudioVCA(FMOD_STUDIO_VCA *instance): Proxy(instance) {}
        makeGetter(GetID, FMOD_GUID);
        makeStringGetter(GetPath);
        make2FloatGetter(GetVolume);
        makeMethod1(SetVolume, float);
    #undef currentClass
    #undef currentType
    };

    class StudioBus: public Proxy<FMOD_STUDIO_BUS> {
    #define currentClass Bus
    #define currentType FMOD_STUDIO_BUS
    public:
        StudioBus(FMOD_STUDIO_BUS *instance): Proxy(instance) {}
        makeCastGetter(GetChannelGroup, ChannelGroup, FMOD_CHANNELGROUP*);
        makeGetter(GetID, FMOD_GUID);
        makeCastGetter(GetMute, bool, FMOD_BOOL);
        makeStringGetter(GetPath);
        makeCastGetter(GetPaused, bool, FMOD_BOOL);
        make2FloatGetter(GetVolume);
        makeMethod(LockChannelGroup);
        void SetMute(bool arg1, lua_State* L) {
            ensure(currentLib, makeFname(SetMute), FMOD_RESULT, currentType*, FMOD_BOOL);
            errCheck(makeFname(SetMute)(instance, arg1));
        }
        void SetPaused(bool arg1, lua_State* L) {
            ensure(currentLib, makeFname(SetPaused), FMOD_RESULT, currentType*, FMOD_BOOL);
            errCheck(makeFname(SetPaused)(instance, arg1));
        }
        makeMethod1(SetVolume, float);
        makeMethod1(StopAllEvents, FMOD_STUDIO_STOP_MODE);
        makeMethod(UnlockChannelGroup);
    #undef currentClass
    #undef currentType
    };

    class StudioBank: public Proxy<FMOD_STUDIO_BANK> {
    #define currentClass Bank
    #define currentType FMOD_STUDIO_BANK
    public:
        StudioBank(FMOD_STUDIO_BANK *instance): Proxy(instance) {}
        makeGetter(GetBusCount, int);
        makeListGetter(GetBusList, StudioBus, FMOD_STUDIO_BUS, GetBusCount);
        makeGetter(GetEventCount, int);
        makeListGetter(GetEventList, StudioEventDescription, FMOD_STUDIO_EVENTDESCRIPTION, GetEventCount);
        makeGetter(GetID, FMOD_GUID);
        makeGetter(GetLoadingState, FMOD_STUDIO_LOADING_STATE);
        makeStringGetter(GetPath);
        makeGetter(GetSampleLoadingState, FMOD_STUDIO_LOADING_STATE);
        makeGetter(GetStringCount, int);
        int GetStringInfo(lua_State* L) {
            ensure(currentLib, makeFname(GetStringInfo), FMOD_RESULT, currentType*, int, FMOD_GUID*, char*, int, int*); \
            int index = (int)luaL_checknumber(L, 2);
            int size;
            errCheck(makeFname(GetStringInfo)(instance, index, NULL, NULL, 0, &size));
            FMOD_GUID id;
            char* path = new char[size];
            errCheck(makeFname(GetStringInfo)(instance, index, &id, path, size, NULL));
            luabridge::Stack<FMOD_GUID>::push(L, id);
            lua_pushstring(L, path);
            delete[] path;
            return 2;
        }
        makeGetter(GetVCACount, int);
        makeListGetter(GetVCAList, StudioVCA, FMOD_STUDIO_VCA, GetVCACount);
        makeMethod(LoadSampleData);
        makeMethod(Unload);
        makeMethod(UnloadSampleData);
    #undef currentClass
    #undef currentType
    };

    class StudioEventInstance: public RefCountedProxy<FMOD_STUDIO_EVENTINSTANCE> {
    #define currentClass EventInstance
    #define currentType FMOD_STUDIO_EVENTINSTANCE
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
        void SetPaused(bool arg1, lua_State* L) { // Need to cast bool to FMOD_BOOL
            ensure(currentLib, makeFname(SetPaused), FMOD_RESULT, currentType*, FMOD_BOOL);
            errCheck(makeFname(SetPaused)(instance, arg1));
        }
        makeMethod1(SetPitch, float);
        makeMethod2(SetProperty, FMOD_STUDIO_EVENT_PROPERTY, float);
        makeMethod2(SetReverbLevel, int, float);
        makeMethod1(SetTimelinePosition, int);
        makeMethod1(SetVolume, float);
        makeMethod(Start);
        makeMethod1(Stop, FMOD_STUDIO_STOP_MODE);
        makeMethod(TriggerCue);
    #undef currentClass
    #undef currentType
    };

    class StudioEventDescription: public Proxy<FMOD_STUDIO_EVENTDESCRIPTION> {
    #define currentClass EventDescription
    #define currentType FMOD_STUDIO_EVENTDESCRIPTION
    public:
        StudioEventDescription(FMOD_STUDIO_EVENTDESCRIPTION *instance): Proxy(instance) {};
        makeCastGetter(CreateInstance, StudioEventInstance, FMOD_STUDIO_EVENTINSTANCE*);
        makeGetter(GetID, FMOD_GUID);
        makeGetter(GetInstanceCount, int);
        makeListGetter(GetInstanceList, StudioEventInstance, FMOD_STUDIO_EVENTINSTANCE, GetInstanceCount);
        makeGetter(GetLength, int);
        makeGetter(GetMaximumDistance, float);
        makeGetter(GetMinimumDistance, float);
        makeGetter1(GetParameter, FMOD_STUDIO_PARAMETER_DESCRIPTION, const char*);
        makeGetter1(GetParameterByIndex, FMOD_STUDIO_PARAMETER_DESCRIPTION, int);
        makeGetter(GetParameterCount, int);
        makeStringGetter(GetPath);
        makeGetter(GetSampleLoadingState, FMOD_STUDIO_LOADING_STATE);
        makeGetter(GetSoundSize, float);
        makeGetter1(GetUserProperty, FMOD_STUDIO_USER_PROPERTY, const char*);
        makeGetter1(GetUserPropertyByIndex, FMOD_STUDIO_USER_PROPERTY, int);
        makeGetter(GetUserPropertyCount, int);
        makeCastGetter(HasCue, bool, FMOD_BOOL);
        makeCastGetter(Is3D, bool, FMOD_BOOL);
        makeCastGetter(IsOneshot, bool, FMOD_BOOL);
        makeCastGetter(IsSnapshot, bool, FMOD_BOOL);
        makeCastGetter(IsStream, bool, FMOD_BOOL);
        makeMethod(LoadSampleData);
        makeMethod(ReleaseAllInstances);
        makeMethod(UnloadSampleData);
    #undef currentClass
    #undef currentType
    };

    #define currentClass EventInstance
    #define currentType FMOD_STUDIO_EVENTINSTANCE
    defineCastGetter(StudioEventInstance::GetDescription, GetDescription, StudioEventDescription, FMOD_STUDIO_EVENTDESCRIPTION*);
    #undef currentClass
    #undef currentType

    class StudioCommandReplay: public RefCountedProxy<FMOD_STUDIO_COMMANDREPLAY> {
    #define currentClass CommandReplay
    #define currentType FMOD_STUDIO_COMMANDREPLAY
    public:
        makeProxyConstructor(StudioCommandReplay, FMOD_STUDIO_COMMANDREPLAY);
        makeGetter1(GetCommandAtTime, int, float);
        makeGetter(GetCommandCount, int);
        makeGetter1(GetCommandInfo, FMOD_STUDIO_COMMAND_INFO, int);
        std::string GetCommandString(int arg1, lua_State* L) {
            ensure(currentLib, makeFname(GetCommandString), FMOD_RESULT, currentType*, int, const char*, int);
            char* tmp = new char[512];
            errCheck(makeFname(GetCommandString)(instance, arg1, tmp, 512));
            std::string str(tmp);
            delete[] tmp;
            return str;
        }
        int GetCurrentCommand(lua_State* L) {
            ensure(currentLib, makeFname(GetCurrentCommand), FMOD_RESULT, currentType*, int*, float*);
            int commandIndex;
            float currentTime;
            errCheck(makeFname(GetCurrentCommand)(instance, &commandIndex, &currentTime));
            lua_pushnumber(L, commandIndex);
            lua_pushnumber(L, currentTime);
            return 2;
        }
        makeGetter(GetLength, float);
        makeCastGetter(GetPaused, bool, FMOD_BOOL);
        makeGetter(GetPlaybackState, FMOD_STUDIO_PLAYBACK_STATE);
        makeMethod1(SeekToCommand, int);
        makeMethod1(SeekToTime, float);
        makeMethod1(SetBankPath, const char*);
        void SetPaused(bool arg1, lua_State* L) { // Need to cast bool to FMOD_BOOL
            ensure(currentLib, makeFname(SetPaused), FMOD_RESULT, currentType*, FMOD_BOOL);
            errCheck(makeFname(SetPaused)(instance, arg1));
        }
        makeMethod(Start);
        makeMethod(Stop);
    #undef currentClass
    #undef currentType
    };

    class StudioSystem: public Proxy<FMOD_STUDIO_SYSTEM> {
    #define currentClass System
    #define currentType FMOD_STUDIO_SYSTEM
    public:
        StudioSystem(FMOD_STUDIO_SYSTEM* instance): Proxy(instance) {}

        StudioBank LoadBankMemory(LuaHBuffer* buffer, FMOD_STUDIO_LOAD_BANK_FLAGS flags, lua_State* L) {
            ensure(ST, FMOD_Studio_System_LoadBankMemory, FMOD_RESULT,
                FMOD_STUDIO_SYSTEM*, const char*, int, FMOD_STUDIO_LOAD_MEMORY_MODE,
                FMOD_STUDIO_LOAD_BANK_FLAGS, FMOD_STUDIO_BANK**
            );

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

        makeMethod(FlushCommands);
        makeMethod(FlushSampleLoading);
        FMOD_STUDIO_ADVANCEDSETTINGS GetAdvancedSettings(lua_State *L) {
            ensure(currentLib, makeFname(GetAdvancedSettings), FMOD_RESULT, currentType*, FMOD_STUDIO_ADVANCEDSETTINGS*);
            FMOD_STUDIO_ADVANCEDSETTINGS result;
            result.cbsize = sizeof(FMOD_STUDIO_ADVANCEDSETTINGS);
            errCheck(makeFname(GetAdvancedSettings)(instance, &result));
            return result;
        }
        makeCastGetter1(GetBank, StudioBank, FMOD_STUDIO_BANK*, const char*);
        makeCastGetter1(GetBankByID, StudioBank, FMOD_STUDIO_BANK*, const FMOD_GUID*);
        makeGetter(GetBankCount, int);
        makeListGetter(GetBankList, StudioBank, FMOD_STUDIO_BANK, GetBankCount);
        makeGetter(GetBufferUsage, FMOD_STUDIO_BUFFER_USAGE);
        makeCastGetter1(GetBus, StudioBus, FMOD_STUDIO_BUS*, const char*);
        makeCastGetter1(GetBusByID, StudioBus, FMOD_STUDIO_BUS*, const FMOD_GUID*);
        makeGetter(GetCPUUsage, FMOD_STUDIO_CPU_USAGE);
        makeCastGetter1(GetEvent, StudioEventDescription, FMOD_STUDIO_EVENTDESCRIPTION*, const char*);
        makeCastGetter1(GetEventByID, StudioEventDescription, FMOD_STUDIO_EVENTDESCRIPTION*, const FMOD_GUID*);
        makeGetter1(GetListenerAttributes, FMOD_3D_ATTRIBUTES, int);
        makeGetter1(GetListenerWeight, float, int);
        makeGetter(GetNumListeners, int);
        makeGetter1(GetSoundInfo, FMOD_STUDIO_SOUND_INFO, const char*)
        makeCastGetter1(GetVCA, StudioVCA, FMOD_STUDIO_VCA*, const char*);
        makeCastGetter1(GetVCAByID, StudioVCA, FMOD_STUDIO_VCA*, const FMOD_GUID*);
        makeCastGetter2(LoadBankFile, StudioBank, FMOD_STUDIO_BANK*, const char*, FMOD_STUDIO_LOAD_BANK_FLAGS);
        makeCastGetter2(LoadCommandReplay, StudioCommandReplay, FMOD_STUDIO_COMMANDREPLAY*, const char*, FMOD_STUDIO_COMMANDREPLAY_FLAGS);
        makeGetter1(LookupID, FMOD_GUID, const char*);
        makeStringGetter1(LookupPath, const FMOD_GUID*);
        makeMethod(ResetBufferUsage);
        makeMethod2(SetListenerAttributes, int, FMOD_3D_ATTRIBUTES*);
        makeMethod2(SetListenerWeight, int, float);
        makeMethod1(SetNumListeners, int);
        makeMethod2(StartCommandCapture, const char*, FMOD_STUDIO_COMMANDCAPTURE_FLAGS);
        makeMethod(StopCommandCapture);
        makeMethod(UnloadAll);
    #undef currentClass
    #undef currentType
    };

    FMOD_GUID parseID(const char *idString, lua_State* L) {
        ensure(ST, FMOD_Studio_ParseID, FMOD_RESULT, const char*, FMOD_GUID*);
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
            .beginClass<FMOD_CREATESOUNDEXINFO>("CREATESOUNDEXINFO")
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
                .beginClass<FMOD_STUDIO_PARAMETER_DESCRIPTION>("PARAMETER_DESCRIPTION")
                    .addData("name", &FMOD_STUDIO_PARAMETER_DESCRIPTION::name)
                    .addData("index", &FMOD_STUDIO_PARAMETER_DESCRIPTION::index)
                    .addData("minimum", &FMOD_STUDIO_PARAMETER_DESCRIPTION::minimum)
                    .addData("maximum", &FMOD_STUDIO_PARAMETER_DESCRIPTION::maximum)
                    .addData("default_value", &FMOD_STUDIO_PARAMETER_DESCRIPTION::defaultvalue)
                    .addData("type", &FMOD_STUDIO_PARAMETER_DESCRIPTION::type)
                .endClass()
                .beginClass<FMOD_STUDIO_USER_PROPERTY>("USER_PROPERTY")
                    .addData("name", &FMOD_STUDIO_USER_PROPERTY::name)
                    .addData("type", &FMOD_STUDIO_USER_PROPERTY::type)
                    .addData("int_value", &FMOD_STUDIO_USER_PROPERTY::intvalue)
                    .addData("bool_value", &FMOD_STUDIO_USER_PROPERTY::boolvalue)
                    .addData("float_value", &FMOD_STUDIO_USER_PROPERTY::floatvalue)
                    .addData("string_value", &FMOD_STUDIO_USER_PROPERTY::stringvalue)
                .endClass()
                .beginClass<FMOD_STUDIO_ADVANCEDSETTINGS>("ADVANCEDSETTINGS")
                    .addData("command_queue_size", &FMOD_STUDIO_ADVANCEDSETTINGS::commandqueuesize)
                    .addData("handle_initial_size", &FMOD_STUDIO_ADVANCEDSETTINGS::handleinitialsize)
                    .addData("studio_update_period", &FMOD_STUDIO_ADVANCEDSETTINGS::studioupdateperiod)
                    .addData("idle_sample_data_pool_size", &FMOD_STUDIO_ADVANCEDSETTINGS::idlesampledatapoolsize)
                .endClass()
                .beginClass<FMOD_STUDIO_BUFFER_INFO>("BUFFER_INFO")
                    .addData("current_usage", &FMOD_STUDIO_BUFFER_INFO::currentusage)
                    .addData("peak_usage", &FMOD_STUDIO_BUFFER_INFO::peakusage)
                    .addData("capacity", &FMOD_STUDIO_BUFFER_INFO::capacity)
                    .addData("stall_count", &FMOD_STUDIO_BUFFER_INFO::stallcount)
                    .addData("stall_time", &FMOD_STUDIO_BUFFER_INFO::stalltime)
                .endClass()
                .beginClass<FMOD_STUDIO_BUFFER_USAGE>("BUFFER_USAGE")
                    .addData("studio_command_queue", &FMOD_STUDIO_BUFFER_USAGE::studiocommandqueue)
                    .addData("studio_handle", &FMOD_STUDIO_BUFFER_USAGE::studiohandle)
                .endClass()
                .beginClass<FMOD_STUDIO_CPU_USAGE>("CPU_USAGE")
                    .addData("dsp_usage", &FMOD_STUDIO_CPU_USAGE::dspusage)
                    .addData("stream_usage", &FMOD_STUDIO_CPU_USAGE::streamusage)
                    .addData("geometry_usage", &FMOD_STUDIO_CPU_USAGE::geometryusage)
                    .addData("update_usage", &FMOD_STUDIO_CPU_USAGE::updateusage)
                    .addData("studio_usage", &FMOD_STUDIO_CPU_USAGE::studiousage)
                .endClass()
                .beginClass<FMOD_STUDIO_SOUND_INFO>("SOUND_INFO")
                    .addData("name_or_data", &FMOD_STUDIO_SOUND_INFO::name_or_data)
                    .addData("mode", &FMOD_STUDIO_SOUND_INFO::mode)
                    .addData("exinfo", &FMOD_STUDIO_SOUND_INFO::exinfo)
                    .addData("sub_sound_index", &FMOD_STUDIO_SOUND_INFO::subsoundindex)
                .endClass()
                .beginClass<FMOD_STUDIO_COMMAND_INFO>("COMMAND_INFO")
                    .addData("command_name", &FMOD_STUDIO_COMMAND_INFO::commandname)
                    .addData("parent_command_index", &FMOD_STUDIO_COMMAND_INFO::parentcommandindex)
                    .addData("frame_number", &FMOD_STUDIO_COMMAND_INFO::framenumber)
                    .addData("frame_time", &FMOD_STUDIO_COMMAND_INFO::frametime)
                    .addData("instance_type", &FMOD_STUDIO_COMMAND_INFO::instancetype)
                    .addData("output_type", &FMOD_STUDIO_COMMAND_INFO::outputtype)
                    .addData("instance_handle", &FMOD_STUDIO_COMMAND_INFO::instancehandle)
                    .addData("output_handle", &FMOD_STUDIO_COMMAND_INFO::outputhandle)
                .endClass()
                .beginClass<StudioBus>("Bus")
                    .addFunction("get_channel_group", &StudioBus::GetChannelGroup)
                    .addFunction("get_id", &StudioBus::GetID)
                    .addFunction("get_mute", &StudioBus::GetMute)
                    .addFunction("get_path", &StudioBus::GetPath)
                    .addFunction("get_paused", &StudioBus::GetPaused)
                    .addCFunction("get_volume", &StudioBus::GetVolume)
                    .addFunction("lock_channel_group", &StudioBus::LockChannelGroup)
                    .addFunction("set_mute", &StudioBus::SetMute)
                    .addFunction("set_paused", &StudioBus::SetPaused)
                    .addFunction("set_volume", &StudioBus::SetVolume)
                    .addFunction("stop_all_events", &StudioBus::StopAllEvents)
                    .addFunction("unlock_channel_group", &StudioBus::UnlockChannelGroup)
                .endClass()
                .beginClass<StudioBank>("Bank")
                    .addFunction("get_bus_count", &StudioBank::GetBusCount)
                    .addCFunction("get_bus_list", &StudioBank::GetBusList)
                    .addFunction("get_event_count", &StudioBank::GetEventCount)
                    .addCFunction("get_event_list", &StudioBank::GetEventList)
                    .addFunction("get_id", &StudioBank::GetID)
                    .addFunction("get_loading_state", &StudioBank::GetLoadingState)
                    .addFunction("get_path", &StudioBank::GetPath)
                    .addFunction("get_sample_loading_state", &StudioBank::GetSampleLoadingState)
                    .addFunction("get_string_count", &StudioBank::GetStringCount)
                    .addCFunction("get_string_info", &StudioBank::GetStringInfo)
                    .addFunction("get_vca_count", &StudioBank::GetVCACount)
                    .addCFunction("get_vca_list", &StudioBank::GetVCAList)
                    .addFunction("load_sample_data", &StudioBank::LoadSampleData)
                    .addFunction("unload", &StudioBank::Unload)
                    .addFunction("unload_sample_data", &StudioBank::UnloadSampleData)
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
                    .addFunction("get_id", &StudioEventDescription::GetID)
                    .addFunction("get_instance_count", &StudioEventDescription::GetInstanceCount)
                    .addCFunction("get_instance_list", &StudioEventDescription::GetInstanceCount)
                    .addFunction("get_length", &StudioEventDescription::GetLength)
                    .addFunction("get_maximum_distance", &StudioEventDescription::GetMaximumDistance)
                    .addFunction("get_minimum_distance", &StudioEventDescription::GetMinimumDistance)
                    .addFunction("get_parameter", &StudioEventDescription::GetParameter)
                    .addFunction("get_parameter_by_index", &StudioEventDescription::GetParameterByIndex)
                    .addFunction("get_parameter_count", &StudioEventDescription::GetParameterCount)
                    .addFunction("get_path", &StudioEventDescription::GetPath)
                    .addFunction("get_sample_loading_state", &StudioEventDescription::GetSampleLoadingState)
                    .addFunction("get_sound_size", &StudioEventDescription::GetSoundSize)
                    .addFunction("get_user_property", &StudioEventDescription::GetUserProperty)
                    .addFunction("get_user_property_by_index", &StudioEventDescription::GetUserPropertyByIndex)
                    .addFunction("get_user_property_count", &StudioEventDescription::GetUserPropertyCount)
                    .addFunction("has_cue", &StudioEventDescription::HasCue)
                    .addFunction("is_3d", &StudioEventDescription::Is3D)
                    .addFunction("is_oneshot", &StudioEventDescription::IsOneshot)
                    .addFunction("is_snapshot", &StudioEventDescription::IsSnapshot)
                    .addFunction("is_stream", &StudioEventDescription::IsStream)
                    .addFunction("load_sample_data", &StudioEventDescription::LoadSampleData)
                    .addFunction("release_all_instances", &StudioEventDescription::ReleaseAllInstances)
                    .addFunction("unload_sample_data", &StudioEventDescription::UnloadSampleData)
                .endClass()
                .beginClass<StudioVCA>("VCA")
                    .addFunction("get_id", &StudioVCA::GetID)
                    .addFunction("get_path", &StudioVCA::GetPath)
                    .addCFunction("get_volume", &StudioVCA::GetVolume)
                    .addFunction("set_volume", &StudioVCA::SetVolume)
                .endClass()
                .beginClass<StudioCommandReplay>("CommandReplay")
                    .addFunction("get_command_at_time", &StudioCommandReplay::GetCommandAtTime)
                    .addFunction("get_command_count", &StudioCommandReplay::GetCommandCount)
                    .addFunction("get_command_info", &StudioCommandReplay::GetCommandInfo)
                    .addFunction("get_command_string", &StudioCommandReplay::GetCommandString)
                    .addCFunction("get_current_command", &StudioCommandReplay::GetCurrentCommand)
                    .addFunction("get_length", &StudioCommandReplay::GetLength)
                    .addFunction("get_paused", &StudioCommandReplay::GetPaused)
                    .addFunction("get_playback_state", &StudioCommandReplay::GetPlaybackState)
                    .addFunction("seek_to_command", &StudioCommandReplay::SeekToCommand)
                    .addFunction("seek_to_time", &StudioCommandReplay::SeekToTime)
                    .addFunction("set_bank_path", &StudioCommandReplay::SetBankPath)
                    .addFunction("set_paused", &StudioCommandReplay::SetPaused)
                    .addFunction("start", &StudioCommandReplay::Start)
                    .addFunction("stop", &StudioCommandReplay::Stop)
                .endClass()
                .beginClass<StudioSystem>("System")
                    .addFunction("flush_commands", &StudioSystem::FlushCommands)
                    .addFunction("flush_sample_loading", &StudioSystem::FlushSampleLoading)
                    .addFunction("get_advanced_settings", &StudioSystem::GetAdvancedSettings)
                    .addFunction("get_bank", &StudioSystem::GetBank)
                    .addFunction("get_bank_by_id", &StudioSystem::GetBankByID)
                    .addFunction("get_bank_count", &StudioSystem::GetBankCount)
                    .addCFunction("get_bank_list", &StudioSystem::GetBankList)
                    .addFunction("get_buffer_usage", &StudioSystem::GetBufferUsage)
                    .addFunction("get_bus", &StudioSystem::GetBus)
                    .addFunction("get_bus_by_id", &StudioSystem::GetBusByID)
                    .addFunction("get_cpu_usage", &StudioSystem::GetCPUUsage)
                    .addFunction("get_event", &StudioSystem::GetEvent)
                    .addFunction("get_event_by_id", &StudioSystem::GetEventByID)
                    .addFunction("get_listener_attributes", &StudioSystem::GetListenerAttributes)
                    .addFunction("get_listener_weight", &StudioSystem::GetListenerWeight)
                    .addFunction("get_num_listeners", &StudioSystem::GetNumListeners)
                    .addFunction("get_sound_info", &StudioSystem::GetSoundInfo)
                    .addFunction("get_vca", &StudioSystem::GetVCA)
                    .addFunction("get_vca_by_id", &StudioSystem::GetVCAByID)
                    .addFunction("load_bank_memory", &StudioSystem::LoadBankMemory)
                    .addFunction("load_bank_file", &StudioSystem::LoadBankFile)
                    .addFunction("load_command_replay", &StudioSystem::LoadCommandReplay)
                    .addFunction("lookup_id", &StudioSystem::LookupID)
                    .addFunction("lookup_path", &StudioSystem::LookupPath)
                    .addFunction("reset_buffer_usage", &StudioSystem::ResetBufferUsage)
                    .addFunction("set_listener_attributes", &StudioSystem::SetListenerAttributes)
                    .addFunction("set_listener_weight", &StudioSystem::SetListenerWeight)
                    .addFunction("set_num_listeners", &StudioSystem::SetNumListeners)
                    .addFunction("start_command_capture", &StudioSystem::StartCommandCapture)
                    .addFunction("stop_command_capture", &StudioSystem::StopCommandCapture)
                    .addFunction("unload_all", &StudioSystem::UnloadAll)
                .endClass()
                .addVariable("system", &FMODBridge::system, false)
            .endNamespace()
        .endNamespace();
}
