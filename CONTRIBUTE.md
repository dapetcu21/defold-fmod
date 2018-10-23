# Contribution guide

If you have some improvements to bring the project, feel free to open a PR.

Please stick to the indentation style used throughout the project (K&R-like).

## Compiling the bridge library

Most of the extension is implemented in a separately-compiled bridge library.
When changing something, the bridge library must be recompiled for all the
platforms affected by the change.

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
make -f Makefile.osx
make -f Makefile.linux
```

### Windows

Install Visual Studio 2017, then open `bridge/vs_proj/fmodbridge.sln`. Build
the `Release x86` and `Release x64` targets.

### Android

Install Android NDK and Java 8, then generate a standalone toolchain:

```bash
$ANDROID_NDK_HOME/build/tools/make_standalone_toolchain.py --arch arm --install-dir /path/to/android-ndk-arm-standalone
```

Then, to build the bridge:

```bash
cd bridge
make -f Makefile.android STANDALONE_NDK=/path/to/android-ndk-arm-standalone
```

### HTML5

[Install and activate Emscripten](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html) 1.38.12:

```
git clone https://github.com/juj/emsdk.git
cd emsdk
./emsdk install sdk-1.38.12-64bit
./emsdk activate sdk-1.38.12-64bit
source ./emsdk_env.sh
```

Then, in this repo:

```bash
cd bridge
make -f Makefile.emscripten
```
