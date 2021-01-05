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

Alternatively, if you don't want to install Python on your system, you can run
it through Docker:

```
docker-compose -f bridge/docker-compose.generate_bindings.yml up
```

The bridge library comes with compilation scripts for each platform:

### macOS & iOS

For both iOS and macOS, you'll need to install Xcode, open it, then accept the
license agreement and let it install the CLI tools. Then:

```bash
cd bridge
./build_darwin.sh
```

Or, for iOS and macOS separately:

```bash
cd bridge
make -f Makefile.osx
make -f Makefile.ios
```

### Windows

Install [Visual Studio 2019 Build Tools](https://visualstudio.microsoft.com/downloads/)
and [GNU Make](http://gnuwin32.sourceforge.net/packages/make.htm), then run `build_windows.bat`.

### Linux, Android, HTML5

You can build all of these in one go with Docker:

```bash
cd bridge
docker-compose up
```

Or use the respective Makefiles to build manually. Make sure to match
the SDK versions that the Docker containers use.

## Updating FMOD to a newer version

This just boils down to downloading all the FMOD distributions and copying the
libraries to the appropriate locations. A script is provided for the second part.

Download all the distributions to the same directory from 
https://fmod.com/download (make sure to uncheck "Open safe files after 
downloading" if using Safari), then, on macOS, run `update_fmod.sh` with the
common prefix of the files you downloaded as the first argument:

```bash
# For example, if you downloaded FMOD 2.01.07:
./update_fmod.sh ~/Downloads/fmodstudioapi20107
```