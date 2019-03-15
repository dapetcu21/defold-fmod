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

First, add `https://github.com/dapetcu21/defold-fmod/archive/master.zip` to
your dependencies in `game.project`.

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
returned a value through a last pointer argument now actually return the value and
throw with an error string when their result is not `FMOD_OK`.

Enums are exposed on the `fmod` table without the leading `FMOD_`. (eg.: `FMOD_STUDIO_PLAYBACK_PLAYING` is exposed as `fmod.STUDIO_PLAYBACK_PLAYING`)

A fully initialised instance of `FMOD::Studio::System` is exposed to Lua as `fmod.studio.system` and the corresponding instance of `FMOD::System` (the low level system), is exposed as `fmod.system`.

See an [example script][example] to get an idea.

Refer to the [FMOD API Documentation] for details about the available APIs.

Not all APIs are exposed yet. For a full list of exposed functions, see [bridge/src/fmod_classes.cpp](bridge/src/fmod_classes.cpp).

Here's some sample code:

```lua
fmod.studio.system:load_bank_memory(resource.load("/banks/Master Bank.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)
fmod.studio.system:load_bank_memory(resource.load("/banks/Master Bank.strings.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)
fmod.studio.system:load_bank_memory(resource.load("/banks/Vehicles.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)

local event_description = fmod.studio.system:get_event("event:/Vehicles/Basic Engine")
local event = event_description:create_instance()
event:start()
```

## Studio Live Update

Add the following setting to your `game.project` to enable the live update server
used by FMOD Studio.

```
[fmod]
live_update = 1
```

## Lua Headers (EmmyLua)
[EmmyLua](https://github.com/EmmyLua) is lua plugin for [IntelliJ IDEA](https://www.defold.com/community/projects/128566/) and VSCode.

Add fmod_headers.lua to your project and it will autocomplete fmod.

## Contributing 

See [CONTRIBUTE.md](./CONTRIBUTE.md) for details about how to contribute to this project.]

[example]: ./main/main.script
[FMOD]: https://fmod.com
[FMOD API Documentation]: https://www.fmod.com/resources/documentation-api
[bundle_resources]: https://www.defold.com/manuals/project-settings/#_project
[set_software_format]: https://www.fmod.org/docs/content/generated/FMOD_System_SetSoftwareFormat.html
