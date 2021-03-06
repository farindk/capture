#!/bin/sh

# --- build software

rm -rf build
mkdir build
(cd build ; cmake .. -DCMAKE_BUILD_TYPE=Release ; make)

# --- package "recorder" as AppImage

rm -rf AppDir
mkdir AppDir

EXTRA_QT_PLUGINS=xcb linuxdeploy-x86_64.AppImage --appdir AppDir/ -e build/recorder/recorder --icon-file=app-resources/recorder.png --desktop-file=app-resources/recorder.desktop --plugin gstreamer --plugin qt --output=appimage
