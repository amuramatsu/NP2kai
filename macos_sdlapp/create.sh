#! /bin/sh

APPDIR="$(dirname "$0")"
BUILDDIR="$1"
CURDIR="$PWD"
POSTFIX=_sdl2

if [ -z "$BUILDDIR" ] || [ ! -d "$BUILDDIR" ] ; then
    echo "Usage: $0 NP2kai_sdl_build_dir"
    exit 1
fi

if [ -f "$BUILDDIR/sdlnp2kai" ]; then
    mkdir -p "$APPDIR/Neko Project II kai.app/Contents/MacOS/"
    cp "$BUILDDIR/sdlnp2kai$POSTFIX" "$APPDIR/Neko Project II kai.app/Contents/MacOS/"
    cd "$APPDIR/Neko Project II kai.app/Contents/MacOS/"
    ln -sf "sdlnp2kai$POSTFIX" "Neko Project II kai"
    cd "$CURDIR"
else
    echo "sdlnp2kai is not found"
fi

if [ -f "$BUILDDIR/sdlnp21kai" ]; then
    mkdir -p "$APPDIR/Neko Project 21 kai.app/Contents/MacOS/"
    cp "$BUILDDIR/sdlnp21kai$POSTFIX" "$APPDIR/Neko Project 21 kai.app/Contents/MacOS/"
    cd "$APPDIR/Neko Project 21 kai.app/Contents/MacOS/"
    ln -sf "sdlnp21kai$POSTFIX" "Neko Project 21 kai"
    cd "$CURDIR"
else
    echo "sdlnp21kai is not found"
fi
