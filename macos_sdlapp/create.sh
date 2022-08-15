#! /bin/sh

APPDIR="$(dirname "$0")"
BUILDDIR="$1"
CURDIR="$PWD"

if [ -z "$BUILDDIR" ] || [ ! -d "$BUILDDIR" ] ; then
    echo "Usage: $0 NP2kai_sdl_build_dir"
    exit 1
fi

if [ -f "$BUILDDIR/sdlnp2kai" ]; then
    cp "$BUILDDIR/sdlnp2kai" "$APPDIR/Neko Project II kai.app/Contents/MacOS/"
    cd "$APPDIR/Neko Project II kai.app/Contents/MacOS/"
    ln -sf sdlnp2kai "Neko Project II kai"
    cd "$CURDIR"
else
    echo "sdlnp2kai is not found"
fi

if [ -f "$BUILDDIR/sdlnp21kai" ]; then
    cp "$BUILDDIR/sdlnp21kai" "$APPDIR/Neko Project 21 kai.app/Contents/MacOS/"
    cd "$APPDIR/Neko Project 21 kai.app/Contents/MacOS/"
    ln -sf sdlnp21kai "Neko Project 21 kai"
    cd "$CURDIR"
else
    echo "sdlnp21kai is not found"
fi
