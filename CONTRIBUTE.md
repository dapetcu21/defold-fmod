# Contribution guide

If you have some improvements to bring the project, feel free to open a PR.

Please stick to the indentation style used throughout the project (K&R-like).

## Compiling the bridge library

Most of the extension is implemented in a separately-compiled bridge library.
When changing something, the bridge library must be recompiled for all the
platforms affected by the change.

The implementation of this library is generated from the FMOD headers by a
Python script. To re-generate it, you will need Python 3 with the `pycparser`
and `jinja2` packages installed from PiP:

```
cd bridge
python generate_bindings.py
```

The bridge library comes with compilation scripts for each platform:

### macOS & iOS

For both iOS and macOS, you'll need to install Xcode, open it, then accept the
license agreement and let it install the CLI tools. Then:

```bash
cd bridge
make -f Makefile.osx
make -f Makefile.ios
```

### Linux

On Ubuntu 16.04, install the GCC toolchain with
`sudo apt-get install build-essential`, then:

```bash
cd bridge
make -f Makefile.linux
```

### Windows

Install Visual Studio 2017, then open `bridge/vs_proj/fmodbridge.sln`. Build
the `Release x86` and `Release x64` targets.

### Android

Download and install [Android NDK r17c] and [Java 8], then to build the bridge:

[Android NDK r17c]:https://developer.android.com/ndk/downloads/older_releases.html
[Java 8]:https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html

```bash
cd bridge
make -f Makefile.android ANDROID_NDK=/path/to/android-ndk-r17c
```

### HTML5

[Install and activate Emscripten](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html) 1.39.17:

```
git clone https://github.com/juj/emsdk.git
cd emsdk
./emsdk install sdk-1.39.17-64bit
./emsdk activate sdk-1.39.17-64bit
source ./emsdk_env.sh
```

Then, in this repo:

```bash
cd bridge
make -f Makefile.emscripten
```

## Updating FMOD to a newer version

This just boils down to downloading all the FMOD distributions and copying the
libraries to the appropriate locations. On HTML5, use the upstream W32 distribution:

```
cp path_to_fmodstudioapi/api/studio/lib/upstream/w32/fmodstudio.a  fmod/lib/web/libfmodstudio.a 
```
