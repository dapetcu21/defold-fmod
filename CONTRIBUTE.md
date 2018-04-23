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