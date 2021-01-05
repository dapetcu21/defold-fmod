set "VSCMD_START_DIR=%CD%"
set "PATH=%PATH%;%PROGRAMFILES(x86)%\GnuWin32\bin"

call "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

make -f Makefile.win32

call "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64

make -f Makefile.win64