#!/bin/bash
set -e

PREFIX="${1}"
REPO="$PWD"
TMPDIR=$(mktemp -d)

YELLOW='\033[0;33m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "Unzipping to ${YELLOW}$TMPDIR${NC}"

echo -e "${YELLOW}Updating macOS FMOD...${NC}"
mkdir -p "$TMPDIR/mac"
hdiutil attach "${PREFIX}mac${FMOD_PATCH_POSTFIX}-installer.dmg" -mountpoint "$TMPDIR/mac"
cp "$TMPDIR/mac/FMOD Programmers API/api/core/lib/libfmod.dylib" "$REPO/fmod/res/osx/Contents/MacOS/libfmod.dylib"
cp "$TMPDIR/mac/FMOD Programmers API/api/studio/lib/libfmodstudio.dylib" "$REPO/fmod/res/osx/Contents/MacOS/libfmodstudio.dylib"
hdiutil detach "$TMPDIR/mac"

echo -e "${YELLOW}Updating iOS FMOD...${NC}"
mkdir -p "$TMPDIR/ios"
hdiutil attach "${PREFIX}ios${FMOD_PATCH_POSTFIX}-installer.dmg" -mountpoint "$TMPDIR/ios"
cp "$TMPDIR/ios/FMOD Programmers API/api/core/lib/libfmod_iphoneos.a" "$REPO/fmod/lib/ios/libfmod_iphoneos.a"
cp "$TMPDIR/ios/FMOD Programmers API/api/studio/lib/libfmodstudio_iphoneos.a" "$REPO/fmod/lib/ios/libfmodstudio_iphoneos.a"
cp "$TMPDIR/ios/FMOD Programmers API/api/core/lib/libfmod_iphonesimulator.a" "$REPO/fmod/lib/x86_64-ios/libfmod_iphonesimulator.a"
cp "$TMPDIR/ios/FMOD Programmers API/api/studio/lib/libfmodstudio_iphonesimulator.a" "$REPO/fmod/lib/x86_64-ios/libfmodstudio_iphonesimulator.a"
hdiutil detach "$TMPDIR/ios"

echo -e "${YELLOW}Updating Linux FMOD...${NC}"
mkdir -p "$TMPDIR/linux"
tar xzf "${PREFIX}linux${FMOD_PATCH_POSTFIX}.tar.gz" -C "$TMPDIR/linux" --strip-components 1
cp -L "$TMPDIR/linux/api/core/lib/x86_64/libfmod.so" "$REPO/fmod/res/x86_64-linux/libfmod.so"
cp -L "$TMPDIR/linux/api/studio/lib/x86_64/libfmodstudio.so" "$REPO/fmod/res/x86_64-linux/libfmodstudio.so"

echo -e "${YELLOW}Updating Windows FMOD...${NC}"
mkdir -p "$TMPDIR/win"
unar "${PREFIX}win${FMOD_PATCH_POSTFIX}-installer.exe" -o "$TMPDIR/win"
cp "$TMPDIR/win"/*/*/api/core/lib/x64/fmod.dll "$REPO/fmod/res/x86_64-win32/fmod-x86_64/fmod.dll"
cp "$TMPDIR/win"/*/*/api/studio/lib/x64/fmodstudio.dll "$REPO/fmod/res/x86_64-win32/fmod-x86_64/fmodstudio.dll"
cp "$TMPDIR/win"/*/*/api/core/lib/x86/fmod.dll "$REPO/fmod/res/x86-win32/fmod-x86/fmod.dll"
cp "$TMPDIR/win"/*/*/api/studio/lib/x86/fmodstudio.dll "$REPO/fmod/res/x86-win32/fmod-x86/fmodstudio.dll"

echo -e "${YELLOW}Updating HTML5 FMOD...${NC}"
mkdir -p "$TMPDIR/html5"
unzip "${PREFIX}html5${FMOD_PATCH_POSTFIX}.zip" -d "$TMPDIR/html5"
cp "$TMPDIR/html5"/*/api/studio/lib/upstream/w32/fmodstudio.a $REPO/fmod/lib/web/libfmodstudio.a

echo -e "${YELLOW}Updating Android FMOD...${NC}"
mkdir -p "$TMPDIR/android"
tar xzf "${PREFIX}android${FMOD_PATCH_POSTFIX}.tar.gz" -C "$TMPDIR/android" --strip-components 1
cp -L "$TMPDIR/android/api/core/lib/arm64-v8a/libfmod.so" "$REPO/fmod/res/android/lib/arm64-v8a/libfmod.so"
cp -L "$TMPDIR/android/api/studio/lib/arm64-v8a/libfmodstudio.so" "$REPO/fmod/res/android/lib/arm64-v8a/libfmodstudio.so"
cp -L "$TMPDIR/android/api/core/lib/armeabi-v7a/libfmod.so" "$REPO/fmod/res/android/lib/armeabi-v7a/libfmod.so"
cp -L "$TMPDIR/android/api/studio/lib/armeabi-v7a/libfmodstudio.so" "$REPO/fmod/res/android/lib/armeabi-v7a/libfmodstudio.so"
cp -L "$TMPDIR/android/api/core/lib/fmod.jar" "$REPO/fmod/lib/android/fmod.jar"

echo -e "${YELLOW}Copying FMOD headers...${NC}"
rm -f "$REPO/bridge/include/fmod"*
cp "$TMPDIR/linux/api/core/inc/"*.{h,hpp} "$REPO/bridge/include/"
cp "$TMPDIR/linux/api/studio/inc/"*.{h,hpp} "$REPO/bridge/include/"

rm -rf "$TMPDIR"
echo -e "${GREEN}Done!${NC}"
