#!/bin/bash
# ZBW POS Android Build Script
# Requires: Qt 5.15+ or Qt6 with Android support, Android NDK/SDK

set -e

APP_NAME="zbwpos"
APP_VERSION="1.0.0"
APP_VERSION_CODE=1
BUILD_DIR="build-android"
PACKAGE_DIR="package"

# Android SDK/NDK paths - adjust these for your installation
ANDROID_SDK=${ANDROID_SDK:-"$HOME/Android/sdk"}
ANDROID_NDK=${ANDROID_NDK:-"$ANDROID_SDK/ndk/23.1.7779620"}

# Qt Android path
detect_qt_android() {
    if [ -n "$QT_ANDROID" ]; then
        return
    fi

    local paths=(
        "/opt/Qt/5.15.2/android"
        "/opt/Qt/6.5.0/android_arm64_v8a"
        "$HOME/Qt/5.15.2/android"
        "$HOME/Qt/6.5.0/android_arm64_v8a"
    )

    for path in "${paths[@]}"; do
        if [ -x "$path/bin/qmake" ]; then
            export QT_ANDROID="$path"
            return
        fi
    done
}

detect_qt_android

# Validate environment
if [ -z "$QT_ANDROID" ] || [ ! -x "$QT_ANDROID/bin/qmake" ]; then
    echo "ERROR: Qt for Android not found. Set QT_ANDROID environment variable"
    exit 1
fi

if [ ! -d "$ANDROID_SDK" ]; then
    echo "ERROR: Android SDK not found at $ANDROID_SDK"
    echo "Set ANDROID_SDK environment variable"
    exit 1
fi

if [ ! -d "$ANDROID_NDK" ]; then
    echo "ERROR: Android NDK not found at $ANDROID_NDK"
    echo "Set ANDROID_NDK environment variable"
    exit 1
fi

export ANDROID_SDK_ROOT="$ANDROID_SDK"
export ANDROID_NDK_ROOT="$ANDROID_NDK"

echo "========================================"
echo "ZBW POS Android Build"
echo "Qt: $QT_ANDROID"
echo "SDK: $ANDROID_SDK"
echo "NDK: $ANDROID_NDK"
echo "========================================"

# Clean previous build
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Build for multiple architectures
ARCHITECTURES=("armeabi-v7a" "arm64-v8a")

for ARCH in "${ARCHITECTURES[@]}"; do
    echo ""
    echo "Building for $ARCH..."
    ARCH_BUILD_DIR="$BUILD_DIR/$ARCH"
    mkdir -p "$ARCH_BUILD_DIR"

    cd "$ARCH_BUILD_DIR"

    "$QT_ANDROID/bin/qmake" \
        ../../zbwpos.pro \
        -spec android-clang \
        CONFIG+=release \
        CONFIG+=qtquickcompiler \
        ANDROID_ABIS="$ARCH"

    make -j$(nproc) qmake_all
    make -j$(nproc)

    cd ../..
done

# Build APK using gradlew
echo ""
echo "Building APK..."

# Qt6 uses a different approach - copy gradle template
if [ -d "$BUILD_DIR/android-build" ]; then
    cd "$BUILD_DIR/android-build"
else
    # For Qt5, need to manually set up Android project
    cd "$BUILD_DIR/arm64-v8a"
    make INSTALL_ROOT=../android-build install
    cd ../..
fi

# Sign APK if keystore is provided
if [ -n "$KEYSTORE" ] && [ -f "$KEYSTORE" ]; then
    echo "Signing APK..."
    "$ANDROID_SDK/build-tools/$(ls $ANDROID_SDK/build-tools | tail -1)/apksigner" \
        sign --ks "$KEYSTORE" --ks-key-alias "$KEY_ALIAS" --ks-pass pass:"$KEYSTORE_PASS" \
        "$BUILD_DIR/android-build/build/outputs/apk/release/android-build-release.apk"
fi

# Create output package
mkdir -p "$PACKAGE_DIR"
cp "$BUILD_DIR/android-build/build/outputs/apk/release/"*.apk "$PACKAGE_DIR/" 2>/dev/null || \
    find "$BUILD_DIR" -name "*.apk" -exec cp {} "$PACKAGE_DIR/" \;

# Rename APK
for apk in "$PACKAGE_DIR"/*.apk; do
    mv "$apk" "$PACKAGE_DIR/${APP_NAME}-${APP_VERSION}-android.apk" 2>/dev/null || true
done

echo "========================================"
echo "Build complete"
echo "APK: $PACKAGE_DIR/"
echo "========================================"
