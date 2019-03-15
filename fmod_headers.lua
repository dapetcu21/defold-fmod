fmod = {}

--region enums
--region fmod.STUDIO_EVENT_PROPERTY
fmod.STUDIO_EVENT_PROPERTY_CHANNELPRIORITY = 1
fmod.STUDIO_EVENT_PROPERTY_SCHEDULE_DELAY = 1
fmod.STUDIO_EVENT_PROPERTY_SCHEDULE_LOOKAHEAD = 1
fmod.STUDIO_EVENT_PROPERTY_MINIMUM_DISTANCE = 1
fmod.STUDIO_EVENT_PROPERTY_MAXIMUM_DISTANCE = 1
fmod.STUDIO_EVENT_PROPERTY_MAX = 1
--endregion

--region fmod.STUDIO_INSTANCETYPE
fmod.STUDIO_INSTANCETYPE_NONE = 1
fmod.STUDIO_INSTANCETYPE_SYSTEM = 1
fmod.STUDIO_INSTANCETYPE_EVENTDESCRIPTION = 1
fmod.STUDIO_INSTANCETYPE_EVENTINSTANCE = 1
fmod.STUDIO_INSTANCETYPE_PARAMETERINSTANCE = 1
fmod.STUDIO_INSTANCETYPE_BUS = 1
fmod.STUDIO_INSTANCETYPE_VCA = 1
fmod.STUDIO_INSTANCETYPE_BANK = 1
fmod.STUDIO_INSTANCETYPE_COMMANDREPLAY = 1
--endregion

--region fmod.STUDIO_LOADING_STATE
fmod.STUDIO_LOADING_STATE_UNLOADING = 1
fmod.STUDIO_LOADING_STATE_UNLOADED = 1
fmod.STUDIO_LOADING_STATE_LOADING = 1
fmod.STUDIO_LOADING_STATE_LOADED = 1
fmod.STUDIO_LOADING_STATE_ERROR = 1
--endregion

--region fmod.STUDIO_LOAD_MEMORY_MODE
fmod.STUDIO_LOAD_MEMORY = 1
fmod.STUDIO_LOAD_MEMORY_POINT = 1
fmod.STUDIO_LOAD_MEMORY_MODE = 1
--endregion

--region fmod.STUDIO_PARAMETER_TYPE
fmod.STUDIO_PARAMETER_GAME_CONTROLLED = 1
fmod.STUDIO_PARAMETER_AUTOMATIC_DISTANCE = 1
fmod.STUDIO_PARAMETER_AUTOMATIC_EVENT_CONE_ANGLE = 1
fmod.STUDIO_PARAMETER_AUTOMATIC_EVENT_ORIENTATION = 1
fmod.STUDIO_PARAMETER_AUTOMATIC_DIRECTION = 1
fmod.STUDIO_PARAMETER_AUTOMATIC_ELEVATION = 1
fmod.STUDIO_PARAMETER_AUTOMATIC_LISTENER_ORIENTATION = 1
fmod.STUDIO_PARAMETER_MAX = 1
--endregion

--region fmod.STUDIO_PLAYBACK_STATE
fmod.STUDIO_PLAYBACK_PLAYING = 1
fmod.STUDIO_PLAYBACK_SUSTAINING = 1
fmod.STUDIO_PLAYBACK_STOPPED = 1
fmod.STUDIO_PLAYBACK_STARTING = 1
fmod.STUDIO_PLAYBACK_STOPPING = 1
--endregion

--region fmod.STUDIO_STOP_MODE
fmod.STUDIO_STOP_ALLOWFADEOUT = 1
fmod.STUDIO_STOP_IMMEDIATE = 1
--endregion

--region fmod.STUDIO_USER_PROPERTY_TYPE
fmod.STUDIO_USER_PROPERTY_TYPE_INTEGER = 1
fmod.STUDIO_USER_PROPERTY_TYPE_BOOLEAN = 1
fmod.STUDIO_USER_PROPERTY_TYPE_FLOAT = 1
fmod.STUDIO_USER_PROPERTY_TYPE_STRING = 1
--endregion


--TODO NEED HELP WHERE YOU BINDS FLAGS
--in main.script you use fmod.STUDIO_LOAD_BANK_NORMAL
--FMOD_STUDIO_LOAD_BANK_FLAGS


--endregion

--region lowLevelClasses
---@class FMOD_CHANNELGROUP
---@class FMOD_SYSTEM
--endregion

--region studio API

---@class FMOD_GUID
---@class FMOD_CREATESOUNDEXINFO

---@class FMOD__3D_ATTRIBUTES
---@field position vector3
---@field velocity vector3
---@field forward vector3
---@field up vector3



---@type FMOD_GUID
fmod.GUID = {}
---@type FMOD_CREATESOUNDEXINFO
fmod.CREATESOUNDEXINFO = {}
---@type FMOD__3D_ATTRIBUTES
fmod._3D_ATTRIBUTES = {}


--region studio class

--TODO NEED HELP index or id ?
--https://www.fmod.com/resources/documentation-api?version=2.0&page=studio-api-common.html#fmod_studio_parameter_description
---@class FMOD_PARAMETER_DESCRIPTION
---@field name string Parameter name.
---@field index number Parameter id. (FMOD_STUDIO_PARAMETER_ID)
---@field minimum number Minimum parameter value.
---@field maximum number Maximum parameter value.
---@field default_value number Default parameter value.
---@field type number fmod.STUDIO_PARAMETER_

---@class FMOD_USER_PROPERTY
---@field name string Property name.
---@field type string Property type. fmod.STUDIO_USER_PROPERTY_TYPE_
---@field int_value number Integer value. Only valid when type is FMOD_STUDIO_USER_PROPERTY_TYPE_INTEGER.
---@field bool_value boolean Boolean value. Only valid when type is FMOD_STUDIO_USER_PROPERTY_TYPE_BOOLEAN.
---@field float_value number Floating point value. Only valid when type is FMOD_STUDIO_USER_PROPERTY_TYPE_FLOAT.
---@field string_value string String value. Only valid when type is FMOD_STUDIO_USER_PROPERTY_TYPE_STRING.

--TODO NEED HELP https://www.fmod.com/resources/documentation-api?version=2.0&page=core-api-system.html#fmod_advancedsettings
---@class FMOD_ADVANCEDSETTINGS
---@field command_queue_size number Deprecated.
---@field handle_initial_size string
---@field studio_update_period number
---@field idle_sample_data_pool_size boolean

---@class FMOD_BUFFER_INFO
---@field current_usage number Current buffer usage in bytes.
---@field peak_usage number Peak buffer usage in bytes.
---@field capacity number Buffer capacity in bytes.
---@field stall_count number Cumulative number of stalls due to buffer overflow.
---@field stall_time number Cumulative amount of time stalled due to buffer overflow, in seconds.

---@class FMOD_BUFFER_USAGE
---@field studio_command_queue FMOD_BUFFER_INFO Information for the Studio Async Command buffer.
---@field studio_handle FMOD_BUFFER_INFO Information for the Studio handle table.

---@class FMOD_CPU_USAGE
---@field dsp_usage number Returns the % CPU time taken by DSP processing on the core mixer thread.
---@field stream_usage number Returns the % CPU time taken by stream processing on the core stream thread.
---@field geometry_usage number Returns the % CPU time taken by geometry processing on the core geometry thread.
---@field update_usage number Returns the % CPU time taken by core update, called as part of the studio update.
---@field studio_usage number Returns the % CPU time taken by studio update, called from the studio thread. Does not include core update time.


--TODO NEED HELP FMOD_MODE NOT BINDED?
---@class FMOD_SOUND_INFO
---@field name_or_data string
---@field mode FMOD_MODE TODO NOT BINDED? https://www.fmod.com/resources/documentation-api?version=2.0&page=core-api-common.html#fmod_mode
---@field exinfo FMOD_CREATESOUNDEXINFO Extra information required for loading the sound.
---@field sub_sound_index number Subsound index for loading the sound.

---@class FMOD_COMMAND_INFO
---@field command_name string Fully qualified C++ name of the API function for this command.
---@field parent_command_index number Index of the command that created the instance this command operates on, or -1 if the command does not operate on any instance.
---@field frame_number number Frame the command belongs to.
---@field frame_time number Playback time at which this command will be executed.
---@field instance_type number fmod.STUDIO_INSTANCETYPE Type of object that this command uses as an instance. (FMOD_STUDIO_INSTANCETYPE)
---@field output_type number fmod.STUDIO_INSTANCETYPE Type of object that this command outputs. (FMOD_STUDIO_INSTANCETYPE)
---@field instance_handle number Original handle value of the instance.
---@field output_handle number Original handle value of the instance.


--region FMOD_BUS
---@class FMOD_BUS
local FMOD_BUS = {}

--Retrieves the core channel group.
---@return FMOD_CHANNELGROUP Core channel group. (ChannelGroup)
function FMOD_BUS:get_channel_group() end

---@return FMOD_GUID
function FMOD_BUS:get_id() end
---@return boolean
function FMOD_BUS:get_mute() end
---@return string
function FMOD_BUS:get_path() end
---@return boolean
function FMOD_BUS:get_paused() end

--[[The finalvolume value is calculated by combining the volume set via Studio::Bus::setVolume
 with the bus's default volume and any snapshots or VCAs that affect the bus.
 Volume changes are processed in the Studio system update, so finalvolume
 will be the value calculated by the last update.--]]
---@return number volume;
---@return number finalvolume
function FMOD_BUS:get_volume() end
--[[This function forces the system to create the channel group and keep it available until
 Studio::Bus::unlockChannelGroup is called. See Signal Paths for details.--]]
function FMOD_BUS:lock_channel_group() end
---@param mute boolean
function FMOD_BUS:set_mute(mute) end
---@param pause boolean
function FMOD_BUS:set_paused(pause) end

---@param volume number Negative level inverts the signal. (-inf, inf) Default: 1
function FMOD_BUS:set_volume(volume) end

--Stops all event instances that are routed into the bus.
---@param stop_mode number FMOD_STUDIO_STOP_MODE
function FMOD_BUS:stop_all_events(stop_mode) end
--This function allows the system to destroy the channel group when it is not needed. See Signal Paths for details.
function FMOD_BUS:unlock_channel_group() end
--endregion
--region FMOD_BANK
---@class FMOD_BANK
local FMOD_BANK = {}
---@return number
function FMOD_BANK:get_bus_count() end
---@return FMOD_BUS[]
function FMOD_BANK:get_bus_list() end
---@return number
function FMOD_BANK:get_event_count() end
---@return FMOD_EVENT_DESCRIPTION[]
function FMOD_BANK:get_event_list() end
---@return FMOD_GUID
function FMOD_BANK:get_id() end
---@return number FMOD_STUDIO_LOADING_STATE
function FMOD_BANK:get_loading_state() end
---@return string
function FMOD_BANK:get_path() end
---@return number FMOD_STUDIO_LOADING_STATE
function FMOD_BANK:get_sample_loading_state() end
---@return number
function FMOD_BANK:get_string_count() end
---@return FMOD_GUID
---@return string path
function FMOD_BANK:get_string_info(index) end
---@return number
function FMOD_BANK:get_vca_count() end
---@return FMOD_VCA[]
function FMOD_BANK:get_vca_list() end
--Loads non-streaming sample data for all events in the bank.
function FMOD_BANK:load_sample_data() end
--Unloads the bank.
function FMOD_BANK:unload() end
--Unloads non-streaming sample data for all events in the bank.
function FMOD_BANK:unload_sample_data() end
--endregion

--region FMOD_EVENT_INSTANCE
---@class FMOD_EVENT_INSTANCE
local FMOD_EVENT_INSTANCE = {}
---@return FMOD__3D_ATTRIBUTES
function FMOD_EVENT_INSTANCE:get_3d_attributes() end
---@return FMOD_CHANNELGROUP
function FMOD_EVENT_INSTANCE:get_channel_group() end
--Retrieves the event description.
---@return FMOD_EVENT_DESCRIPTION
function FMOD_EVENT_INSTANCE:get_description() end
---@return number unsigned int
function FMOD_EVENT_INSTANCE:get_listener_mask() end
---@return number
function FMOD_EVENT_INSTANCE:get_parameter_count() end

---@param string string Name of the parameter (case-insensitive).
---@return number value
---@return number finalvalue
function FMOD_EVENT_INSTANCE:get_parameter_value(string) end
---@param index number
---@return number value
---@return number finalvalue
function FMOD_EVENT_INSTANCE:get_parameter_value_by_index(index) end
---@return boolean
function FMOD_EVENT_INSTANCE:get_paused() end
---@return number pitch
---@return number finalpitch
function FMOD_EVENT_INSTANCE:get_pitch() end
---@return number FMOD_STUDIO_PLAYBACK_STATE
function FMOD_EVENT_INSTANCE:get_playback_state() end
---@param property number FMOD_STUDIO_EVENT_PROPERTY
---@return number value
function FMOD_EVENT_INSTANCE:get_property(property) end
---@param reverb number _[0-3]
---@return number level
function FMOD_EVENT_INSTANCE:get_reverb_level(reverb) end
---@return number
function FMOD_EVENT_INSTANCE:get_timeline_position() end
---@return number value
---@return number finalvalue
function FMOD_EVENT_INSTANCE:get_volume() end
---@return boolean
function FMOD_EVENT_INSTANCE:is_virtual() end
---@param attr FMOD__3D_ATTRIBUTES
function FMOD_EVENT_INSTANCE:set_3d_attributes(attr) end

--[[The listener mask controls which listeners are considered when calculating 3D panning
and the values of listener relative automatic parameters.
The default mask of 0xffffffff includes all listeners.--]]
---@param mask number
function FMOD_EVENT_INSTANCE:set_listener_mask(mask) end
---@param name string FMOD_STUDIO_EVENT_PROPERTY
---@param value number
function FMOD_EVENT_INSTANCE:set_parameter_value(name, value) end
---@param index number FMOD_STUDIO_PARAMETER_ID
function FMOD_EVENT_INSTANCE:set_parameter_value_by_index(index) end
---@param paused boolean
function FMOD_EVENT_INSTANCE:set_paused(paused) end

---@param pitch number Units: Linear Range: [0, inf) Default: 1
function FMOD_EVENT_INSTANCE:set_pitch(pitch) end
---@param name number FMOD_STUDIO_EVENT_PROPERTY
---@param value number float
function FMOD_EVENT_INSTANCE:set_property(name,value) end
---@param level number Range:[0, 3]
---@param reverb number Units: Linear Range: [0, 1] Default: 0
function FMOD_EVENT_INSTANCE:set_reverb_level(level, reverb) end
---@param position number
function FMOD_EVENT_INSTANCE:set_timeline_position(position) end
---@param volume number Units: Linear Range: [0, 1] Default: 1
function FMOD_EVENT_INSTANCE:set_volume(volume) end
--Starts playback.
function FMOD_EVENT_INSTANCE:start() end
--Stops playback.
function FMOD_EVENT_INSTANCE:stop() end
--Triggers a cue on the event instance.
function FMOD_EVENT_INSTANCE:trigger_cue() end
--endregion

--region FMOD_EVENT_DESCRIPTION
---@class FMOD_EVENT_DESCRIPTION
local FMOD_EVENT_DESCRIPTION = {}
---@return FMOD_EVENT_INSTANCE
function FMOD_EVENT_DESCRIPTION:create_instance() end
---@return FMOD_GUID
function FMOD_EVENT_DESCRIPTION:get_id() end
---@return number
function FMOD_EVENT_DESCRIPTION:get_instance_count() end
---@return FMOD_EVENT_INSTANCE[]
function FMOD_EVENT_DESCRIPTION:get_instance_list() end
---@return number Units: Milliseconds
function FMOD_EVENT_DESCRIPTION:get_length() end
--[[Retrieves the largest Max Distance value of all Spatializers and 3D Object Spatializers
 on the event's master track. Returns zero if there are no Spatializers or 3D Object Spatializers.--]]
---@return number Maximum distance.
function FMOD_EVENT_DESCRIPTION:get_maximum_distance() end
--[[Retrieves the smallest Min Distance value of all Spatializers and 3D Object Spatializers
 on the event's master track. Returns zero if there are no Spatializers or 3D Object Spatializers.--]]
---@return number
function FMOD_EVENT_DESCRIPTION:get_minimum_distance() end
---@param name string
---@return FMOD_PARAMETER_DESCRIPTION
function FMOD_EVENT_DESCRIPTION:get_parameter(name) end
---@param index string
---@return FMOD_PARAMETER_DESCRIPTION
function FMOD_EVENT_DESCRIPTION:get_parameter_by_index(index) end
---@return number
function FMOD_EVENT_DESCRIPTION:get_parameter_count() end
---@return string
function FMOD_EVENT_DESCRIPTION:get_path() end
---@return number FMOD_STUDIO_LOADING_STATE
function FMOD_EVENT_DESCRIPTION:get_sample_loading_state() end
--Retrieves the sound size for 3D panning.
---@return number
function FMOD_EVENT_DESCRIPTION:get_sound_size() end
---@param name string
---@return FMOD_USER_PROPERTY
function FMOD_EVENT_DESCRIPTION:get_user_property(name) end
---@param index number
---@return FMOD_USER_PROPERTY
function FMOD_EVENT_DESCRIPTION:get_user_property_by_index(index) end
---@return number
function FMOD_EVENT_DESCRIPTION:get_user_property_count() end
---@return boolean
function FMOD_EVENT_DESCRIPTION:has_cue() end
---@return boolean
function FMOD_EVENT_DESCRIPTION:is_3d() end
---@return boolean
function FMOD_EVENT_DESCRIPTION:is_oneshot() end
---@return boolean
function FMOD_EVENT_DESCRIPTION:is_snapshot() end
---@return boolean
function FMOD_EVENT_DESCRIPTION:is_stream() end
--Loads non-streaming sample data used by the event.
function FMOD_EVENT_DESCRIPTION:load_sample_data() end
--Releases all instances.
function FMOD_EVENT_DESCRIPTION:release_all_instances() end
--Unloads all non-streaming sample data.
function FMOD_EVENT_DESCRIPTION:unload_sample_data() end
--endregion

--region FMOD_VCA Represents a global mixer VCA.
---@class FMOD_VCA
local FMOD_VCA = {}

---Retrieves the GUID.
---@return FMOD_GUID
function FMOD_VCA:get_id() end

--Retrieves the path.
---@return string
function FMOD_VCA:get_path() end

--[[The final combined volume returned in finalvolume combines the user value set using Studio::VCA::setVolume
 with the result of any automation or modulation applied to the VCA. The final combined volume is calculated
 asynchronously when the Studio system updates.--]]
---@return number volume;
---@return number finalvolume
function FMOD_VCA:get_volume() end

--The VCA volume level is used to linearly modulate the levels of the buses and VCAs which it controls.
---@param volume number
function FMOD_VCA:set_volume(volume) end
--endregion

--region FMOD_COMMAND_REPLAY
---@class FMOD_COMMAND_REPLAY
local FMOD_COMMAND_REPLAY = {}
--Retrieves the command index corresponding to the given playback time.
--This function will return an index for the first command at or after time. If time is greater than the total playback time then FMOD_ERR_EVENT_NOTFOUND is returned.
---@param time number
---@return number  will return an index or FMOD_ERR_EVENT_NOTFOUND
function FMOD_COMMAND_REPLAY:get_command_at_time(time) end
---@return number
function FMOD_COMMAND_REPLAY:get_command_count() end
---@param index number
---@return FMOD_COMMAND_INFO
function FMOD_COMMAND_REPLAY:get_command_info(index) end
---@param index number
---@return string
function FMOD_COMMAND_REPLAY:get_command_string(index) end
---@return number command index
---@return number current time
function FMOD_COMMAND_REPLAY:get_current_command() end
---@return number The total playback tim
function FMOD_COMMAND_REPLAY:get_length() end
---@return boolean
function FMOD_COMMAND_REPLAY:get_paused() end
---@return number FMOD_STUDIO_PLAYBACK_STATE
function FMOD_COMMAND_REPLAY:get_playback_state() end
---@param index number
function FMOD_COMMAND_REPLAY:seek_to_command(index) end
---@param time number
function FMOD_COMMAND_REPLAY:seek_to_time(time) end
---@param path string
function FMOD_COMMAND_REPLAY:set_bank_path(path) end
---@param paused boolean
function FMOD_COMMAND_REPLAY:set_paused(paused) end
--If the replay is already running then calling this function will restart replay from the beginning.
function FMOD_COMMAND_REPLAY:start() end
--Stops playback.
function FMOD_COMMAND_REPLAY:stop() end
--endregion

---@class FMOD_STUDIO_SYSTEM
local FMOD_STUDIO_SYSTEM = {}
--Block until all pending commands have been executed.
function FMOD_STUDIO_SYSTEM:flush_commands() end
--Block until all sample loading and unloading has completed.
function FMOD_STUDIO_SYSTEM:flush_sample_loading() end
---@return FMOD_ADVANCEDSETTINGS
function FMOD_STUDIO_SYSTEM:get_advanced_settings() end
---@param path string
---@return FMOD_BANK
function FMOD_STUDIO_SYSTEM:get_bank(path) end
---@param id FMOD_GUID
---@return FMOD_BANK
function FMOD_STUDIO_SYSTEM:get_bank_by_id(id) end
---@return number
function FMOD_STUDIO_SYSTEM:get_bank_count() end
---@return FMOD_BANK[]
function FMOD_STUDIO_SYSTEM:get_bank_list() end
---@return FMOD_BUFFER_USAGE
function FMOD_STUDIO_SYSTEM:get_buffer_usage() end
---@param path string
---@return FMOD_BUS
function FMOD_STUDIO_SYSTEM:get_bus(path) end
---@param id FMOD_GUID
---@return FMOD_BANK
function FMOD_STUDIO_SYSTEM:get_bus_by_id(id) end
---@return FMOD_CPU_USAGE
function FMOD_STUDIO_SYSTEM:get_cpu_usage() end
---@param path string
---@return FMOD_EVENT_DESCRIPTION
function FMOD_STUDIO_SYSTEM:get_event(path) end
---@param id FMOD_GUID
---@return FMOD_EVENT_DESCRIPTION
function FMOD_STUDIO_SYSTEM:get_event_by_id(id) end
---@param index number
---@return FMOD__3D_ATTRIBUTES
function FMOD_STUDIO_SYSTEM:get_listener_attributes(index) end
---@param index number
---@return number
function FMOD_STUDIO_SYSTEM:get_listener_weight(index) end
---@return number
function FMOD_STUDIO_SYSTEM:get_num_listeners() end
---@param key string
---@return FMOD_SOUND_INFO
function FMOD_STUDIO_SYSTEM:get_sound_info(key) end
---@param name string
---@return FMOD_VCA
function FMOD_STUDIO_SYSTEM:get_vca(name) end
---@param id FMOD_GUID
---@return FMOD_VCA
function FMOD_STUDIO_SYSTEM:get_vca_by_id(id) end
---@param buffer buffer
---@param flags number fmod.STUDIO_LOAD_BANK_
---@return FMOD_BANK
function FMOD_STUDIO_SYSTEM:load_bank_memory(buffer, flags) end
---@param path string
---@param flags number fmod.STUDIO_LOAD_BANK_
---@return FMOD_BANK
function FMOD_STUDIO_SYSTEM:load_bank_file(path, flags) end
---@param filename string
---@param flags number
---@return FMOD_COMMAND_REPLAY
function FMOD_STUDIO_SYSTEM:load_command_replay(filename,flags) end
---@param path string
---@return FMOD_GUID
function FMOD_STUDIO_SYSTEM:lookup_id(path) end
---@param id number
---@return string
function FMOD_STUDIO_SYSTEM:lookup_path(id) end
--Resets memory buffer usage statistics.
function FMOD_STUDIO_SYSTEM:reset_buffer_usage() end
---@param listener number
---@param attributes FMOD__3D_ATTRIBUTES
function FMOD_STUDIO_SYSTEM:set_listener_attributes(listener,attributes) end
---@param listener number
---@param weight number Range: [0, 1] Default: 1
function FMOD_STUDIO_SYSTEM:set_listener_weight(listener,weight) end
--Sets the number of listeners in the 3D sound scene.
---@param numlisteners number
function FMOD_STUDIO_SYSTEM:set_num_listeners(numlisteners) end

--TODO NEED HELP FMOD_STUDIO_COMMANDCAPTURE_FLAG NOT BINDED?
--Recording Studio commands to a file.
---@param filename string The name of the file to which the recorded commands are written.
---@param flags FMOD_STUDIO_COMMANDCAPTURE_FLAG https://www.fmod.com/resources/documentation-api?version=2.0&page=studio-api-system.html#fmod_studio_commandcapture_flags
function FMOD_STUDIO_SYSTEM:start_command_capture(filename,flags) end
--Stop recording Studio commands.
function FMOD_STUDIO_SYSTEM:stop_command_capture() end
--Unloads all currently loaded banks.
function FMOD_STUDIO_SYSTEM:unload_all() end
--endregion

---@class FMOD_STUDIO
---@field system FMOD_STUDIO_SYSTEM
local FMOD_STUDIO = {}
---@param string string
---@return FMOD_GUID
function FMOD_STUDIO:parse_id(string)

end



---@type FMOD_STUDIO
fmod.studio = {}

--endregion
