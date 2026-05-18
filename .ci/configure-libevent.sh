#!/bin/sh
set -x
set -o errexit

SCRIPT_DIR="$(CDPATH= cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(CDPATH= cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_DIR/event"

if [ -n "${NDK_DIR:-}" ]; then
    ANDROID_API_LEVEL="${ANDROID_API_LEVEL:-26}"
    ANDROID_ABI="${ANDROID_ABI:-arm64-v8a}"
    case "$ANDROID_ABI" in
        armeabi-v7a)
            ANDROID_TARGET="armv7a-linux-androideabi$ANDROID_API_LEVEL"
            ANDROID_HOST="arm-linux-androideabi"
            ;;
        arm64-v8a)
            ANDROID_TARGET="aarch64-linux-android$ANDROID_API_LEVEL"
            ANDROID_HOST="aarch64-linux-android"
            ;;
        x86)
            ANDROID_TARGET="i686-linux-android$ANDROID_API_LEVEL"
            ANDROID_HOST="i686-linux-android"
            ;;
        x86_64)
            ANDROID_TARGET="x86_64-linux-android$ANDROID_API_LEVEL"
            ANDROID_HOST="x86_64-linux-android"
            ;;
        *)
            echo "Unsupported Android ABI: $ANDROID_ABI" >&2
            exit 1
            ;;
    esac
    case "$NDK_DIR" in
        /*)
            NDK_DIR_PATH="$NDK_DIR"
            ;;
        *)
            NDK_DIR_PATH="$PROJECT_DIR/$NDK_DIR"
            ;;
    esac
    NDK_DIR_ABS="$(cd "$NDK_DIR_PATH" && pwd)"
    NDK_PREBUILT_DIR="$(find "$NDK_DIR_ABS/toolchains/llvm/prebuilt" -mindepth 1 -maxdepth 1 -type d | sort | head -n 1)"
    if [ -z "$NDK_PREBUILT_DIR" ]; then
        echo "Android NDK toolchain not found under $NDK_DIR_ABS/toolchains/llvm/prebuilt" >&2
        exit 1
    fi
    NDK_BIN_DIR="$NDK_PREBUILT_DIR/bin"
    export CC="$NDK_BIN_DIR/clang --target=$ANDROID_TARGET"
    export CXX="$NDK_BIN_DIR/clang++ --target=$ANDROID_TARGET"
    export AR="$NDK_BIN_DIR/llvm-ar"
    export RANLIB="$NDK_BIN_DIR/llvm-ranlib"
    ./configure \
        --host="$ANDROID_HOST" \
        --disable-openssl \
        --enable-static=yes \
        --enable-shared=no
else
    ./configure --disable-openssl --enable-static=yes --enable-shared=no
fi

sed -f make-event-config.sed < config.h > ./include/event2/event-config.h
