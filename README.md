![fmod](./docs/fmod_logo.png)

# FMOD extension for Defold

> **Experimental** FMOD bindings for Defold. Currently macOS-only.

## Installation

~~As usual, add `https://github.com/dapetcu21/defold-fmod/master.zip` to
dependencies in `game.project`.~~

Because the FMOD license doesn't allow me to distribute FMOD with this package, you'll have to:

1. Download FMOD API 1.10.02.
2. Fork this repo.
3. Copy the FMOD headers from `api/lowlevel/inc` and `api/studio/inc` to `fmod/include`.
4. Copy the FMOD libraries from `api/lowlevel/libfmod.dylib` and `api/studio/libfmodstudio.dylib` to `fmod/lib/x86_64-osx`.
5. Change the install paths of the libraries (workaround for DEF-2732):
  ```bash
  install_name_tool -id @executable_path/libfmod.dylib libfmod.dylib
  install_name_tool -id @executable_path/libfmodstudio.dylib libfmodstudio.dylib
  install_name_tool -change @rpath/libfmod.dylib @executable_path/libfmod.dylib libfmodstudio.dylib
  ```
6. Copy the libraries you just patched to `fmod/lib/x86_64-osx/Contents/MacOS` (workaround for DEF-2732).
7. Make a zip out of the whole thing and add its URL to dependencies in `game.project`.

## Running

Defold doesn't currently support dynamic libraries (DEF-2732), so a hacky
workaround is needed to run the game in the editor.

The game will bundle fine, but will not start in the editor unless you start
the editor with:

```bash
DYLD_LIBRARY_PATH="/path/to/defold-fmod/fmod/lib/x86_64-osx" /path/to/Defold.app/Contents/MacOS/Defold
```

## Usage

> TODO. Meanwhile, see example.
