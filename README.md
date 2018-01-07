![fmod](./docs/fmod_logo.png)

# FMOD extension for Defold

> **Experimental** [FMOD] sound engine bindings for Defold. Supports macOS and Windows

## Installation

First, add `https://github.com/dapetcu21/defold-fmod/master.zip` to
your dependencies in `game.project`.

Because the FMOD license doesn't allow me to distribute FMOD with this package,
a few extra steps are required for each platform:

### macOS

1. Download FMOD API 1.10.x for macOS.
2. Providing `<resources>` is the name of your [`bundle_resources`][bundle_resources]
  directory, copy the FMOD libraries from `api/lowlevel/lib/libfmod.dylib` and
  `api/studio/lib/libfmodstudio.dylib` to `<resources>/x86_64-osx/Contents/MacOS/`.

### Windows

1. Download FMOD API 1.10.x for Windows.
2. Create a new directory somewhere in your project which we'll refer to as `<stub>`.
3. Create a file at `<stub>/ext.manifest` containing just `name: "DefoldFMODStub"`.
4. Create an empty file at `<stub>/src/stub.cpp`.
5. Copy the FMOD libraries from `api/lowlevel/lib/fmod_vc.lib` and
  `api/studio/lib/fmodstudio_vc.lib` to `<stub>/lib/x86-win32/`.
6. Copy `api/lowlevel/lib/fmod64_vc.lib` and `api/studio/lib/fmodstudio64_vc.lib`
  to `<stub>/lib/x86_64-win32/`.
7. Copy `api/lowlevel/lib/fmod.dll` and `api/studio/lib/fmodstudio.dll`
  to `<stub>/res/x86-win32/`.
8. Copy `api/lowlevel/lib/fmod64.dll` and `api/studio/lib/fmodstudio64.dll`
  to `<stub>/res/x86_64-win32/`.

## Running

The game will bundle fine, but in order for FMOD to be available when running
from the editor, an extra step is required:

### macOS

1. Copy `libfmod.dylib`, `libfmodbridge.dylib` and `libfmodstudio.dylib` from
  a bundled macOS build of your game to a directory on your file system.
2. Open `/etc/launchd.conf` in a text editor. You might need to create the file.
  You also might need `sudo`.
3. Add `setenv DEFOLD_FMOD_LIB_PATH /path/to/the/libs` at the end of the file.
4. Restart your computer (Or run `launchctl setenv DEFOLD_FMOD_LIB_PATH /path/to/the/libs` and just restart Defold).

### Windows

Copy `fmod64.dll` and `fmodstudio64.dll` to `C:\Windows\System32`. If you don't
do this the game will crash at startup, as opposed from macOS and Linux where
the game just starts without FMOD support.

## Usage

Structs and classes are exposed on the `fmod` and `fmod.studio` namespaces. All
method names are converted from `camelCase` to `snake_case`. Methods that
returned a value through a last pointer argument now actually return the value and
throw with an error string when their result is not `FMOD_OK`.

Enums are exposed on the `fmod` table without the leading `FMOD_`. (eg.: `FMOD_STUDIO_PLAYBACK_PLAYING` is exposed as `fmod.STUDIO_PLAYBACK_PLAYING`)

A fully initialised instance of `FMOD::Studio::System` is exposed to Lua as `fmod.studio.system` and the corresponding instance of `FMOD::System` (the low level system), is exposed as `fmod.system`.

See an [example script][example] to get an idea.

Refer to the [FMOD API Documentation] for details about the available APIs.

Not all APIs are exposed yet. For a full list of exposed functions, see [fmod/src/fmod_classes.cpp#L161](fmod/src/fmod_classes.cpp#L161).

Here's some sample code:

```lua
fmod.studio.system:load_bank_memory(resource.load("/resources/Master Bank.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)
fmod.studio.system:load_bank_memory(resource.load("/resources/Master Bank.strings.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)
fmod.studio.system:load_bank_memory(resource.load("/resources/Vehicles.bank"), fmod.STUDIO_LOAD_BANK_NORMAL)

local event_description = fmod.studio.system:get_event("event:/Vehicles/Basic Engine")
local event = event_description:create_instance()
event:start()
```

[example]: ./main/main.script
[FMOD]: https://fmod.com
[FMOD API Documentation]: https://www.fmod.com/resources/documentation-api
[bundle_resources]: https://www.defold.com/manuals/project-settings/#_project
