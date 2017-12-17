![fmod](./docs/fmod_logo.png)

# FMOD extension for Defold

> **Experimental** FMOD bindings for Defold. Currently macOS-only.

## Installation

As usual, add `https://github.com/dapetcu21/defold-fmod/master.zip` to
dependencies in `game.project`.

Defold doesn't currently support dynamic libraries (DEF-2732), so a hacky
workaround is needed.

The game will bundle fine, but will not start in the editor unless you start
the editor with:

```bash
DYLD_LIBRARY_PATH="/path/to/defold-fmod/fmod/lib/x86_64-osx" /path/to/Defold.app/Contents/MacOS/Defold
```

## Usage

> TODO. Meanwhile, see example.
