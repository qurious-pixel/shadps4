#!/bin/bash

if [[ -z $GITHUB_WORKSPACE ]]; then
	GITHUB_WORKSPACE="${PWD%/*}"
fi

QMAKE6=$(find -name qmake)
echo "$QMAKE6"

# Prepare Tools for building the AppImage
wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage

chmod a+x linuxdeploy-x86_64.AppImage
chmod a+x linuxdeploy-plugin-qt-x86_64.AppImage

# Build AppImage
QMAKE="$QMAKE6" ./linuxdeploy-x86_64.AppImage --appdir AppDir -d "$GITHUB_WORKSPACE"/.github/shadps4.desktop  -e "$GITHUB_WORKSPACE"/build/src/shadps4 -i "$GITHUB_WORKSPACE"/.github/shadps4.png --plugin qt --output appimage
