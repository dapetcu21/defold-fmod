![fmod](./docs/fmod_logo.png)

# FMOD extension for Defold

> **Experimental** [FMOD] sound engine bindings for Defold

**Commercial usage of FMOD products may require a separate license directly with
Firelight Technologies. For details refer to [fmod.com/resources/eula][fmod-license].**

**According to the [FMOD license][fmod-license], you must credit FMOD in your
project's documentation, or in an on screen format. The text should include at
least the words "FMOD" (OR "FMOD STUDIO" IF APPLICABLE) AND "FIRELIGHT TECHNOLOGIES."**

[fmod-license]: https://fmod.com/resources/eula

## Installation

Go to the [Releases page](https://github.com/dapetcu21/defold-fmod/releases),
copy a dependency URL, then add it to your dependencies in `game.project`.

### Set the speaker mode

This step is [only required if you use Studio banks][set_software_format].
To set the speaker mode, create an `fmod` section in your `game.project`:

```
[fmod]
speaker_mode = stereo
```

Supported values for `speaker_mode` are: `default`, `raw`, `mono`, `stereo`,
`quad`, `surround`, `5.1`, `7.1`, `max`.

If ever needed, you can also set `sample_rate` and `num_raw_speakers` in the same way.

### Running in the editor

The game will bundle fine, but in order for FMOD to be available when running
from the editor, an extra step is required.

Copy the `fmod/res` directory from this repo to a directory in your project
and add the path to that directory to your `game.project`:

```
[fmod]
lib_path = path/to/fmod/res
```

## Usage

Structs and classes are exposed on the `fmod` and `fmod.studio` namespaces. All
method names are converted from `camelCase` to `snake_case`. Methods that
returned values through pointer arguments now actually return the values and
throw with a Lua error when their result is not `FMOD_OK`.

Enums are exposed on the `fmod` table without the leading `FMOD_`.
(eg.: `FMOD_STUDIO_PLAYBACK_PLAYING` is exposed as `fmod.STUDIO_PLAYBACK_PLAYING`)

A fully initialised instance of `FMOD::Studio::System` is exposed to Lua as
`fmod.studio.system` and the corresponding instance of
`FMOD::System` (the low level system), is exposed as `fmod.system`.

You can use `vmath.vector3` instead of FMOD's `FMOD_VECTOR` struct. Conversion
is being done seamlessly.

See an [example script][example] to get an idea.

Refer to the [FMOD API Documentation] for details about the available APIs.

Here's some sample code:

```lua
fmod.studio.system:load_bank_memory(resource.load("/banks/Master Bank.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)
fmod.studio.system:load_bank_memory(resource.load("/banks/Master Bank.strings.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)
fmod.studio.system:load_bank_memory(resource.load("/banks/Vehicles.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)

local event_description = fmod.studio.system:get_event("event:/Vehicles/Basic Engine")
local event = event_description:create_instance()
event:start()
```

## Memory management

Instances of `FMOD_STUDIO_EVENTINSTANCE` are automatically garbage collected by
Lua and `release()` is called automatically for you when you no longer hold
references.

**Any other class is not memory managed and you still need to call `instance:release()`
manually, where applicable.**

## Error handling

You can retrieve the error code of an FMOD error and use it for more specific
error handling:

```lua
local ok, err = pcall(function ()
  return fmod.studio.system:get_event("event:/Inexistent event")
end)
if ok then
  local event_description = err
  event_description:create_instance():start()
else
  print(err) -- The error is a string
  local code = fmod.error_code[err] -- The FMOD error code (a number).
  print(code)
  if code == fmod.ERR_EVENT_NOTFOUND then
    print("Ooops! This event doesn't exist!")
  end
end
```

## 64-bit values

Since Lua 5.1 doesn't have a 64-bit type, you can use `fmod.s64()` and `fmod.u64()`
to create 64-bit values.

```lua
x = fmod.s64(num) -- Converts a Lua number (up to 52bits of integer precision) to a 64-bit value
x = fmod.s64(low, high) -- Creates a 64-bit value from two 32-bit integers
x.value -- Does the best-effort conversion to a Lua number (accurate within 52 bits of precision)
x.low -- Gets the lowest 32-bits as an unsigned int value
x.high -- Gets the lowest 32-bits as an unsigned int value
tostring(x) -- Converts the value to a numeric string
```

## Studio Live Update

Add the following setting to your `game.project` to enable the live update server
used by FMOD Studio.

```
[fmod]
live_update = 1
```

## Contributing

See [CONTRIBUTE.md](./CONTRIBUTE.md) for details about how to contribute to this project.

[example]: ./main/main.script
[FMOD]: https://fmod.com
[FMOD API Documentation]: https://www.fmod.com/resources/documentation-api?version=2.0&page=content/generated/studio_api.html
[bundle_resources]: https://www.defold.com/manuals/project-settings/#_project
[set_software_format]: https://www.fmod.org/docs/content/generated/FMOD_System_SetSoftwareFormat.html
