#!/bin/bash
# ZBW POS macOS Build Script
# Requires: Qt 5.15+ or Qt6, Xcode Command Line Tools

set -e

APP_NAME="zbwpos"
APP_VERSION="1.0.0"
BUILD_DIR="build-macos"
INSTALL_DIR="install"

# Detect Qt installation
detect_qt() {
    if [ -n "$QT_DIR" ]; then
        return
    fi

    local paths=(
        "/opt/Qt/5.15.2/clang_64"
        "/opt/Qt/6.5.0/macos"
        "/usr/local/Qt-5.15.2"
        "/usr/local/Qt-6.5.0"
        "$HOME/Qt/5.15.2/clang_64"
        "$HOME/Qt/6.5.0/macos"
    )

    for path in "${paths[@]}"; do
        if [ -x "$path/bin/qmake" ]; then
            export QT_DIR="$path"
            export PATH="$path/bin:$PATH"
            return
        fi
    done
}

detect_qt

if [ -z "$QT_DIR" ] || [ ! -x "$QT_DIR/bin/qmake" ]; then
    echo "ERROR: Qt not found. Please install Qt or set QT_DIR environment variable"
    exit 1
fi

# Get deployment target
MACOSX_DEPLOYMENT_TARGET=${MACOSX_DEPLOYMENT_TARGET:-"10.14"}
export MACOSX_DEPLOYMENT_TARGET

echo "========================================"
echo "ZBW POS macOS Build"
echo "Qt: $QT_DIR"
echo "Deployment Target: $MACOSX_DEPLOYMENT_TARGET"
echo "========================================"

# Clean previous build
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Build
cd "$BUILD_DIR"
echo "Running qmake..."
"$QT_DIR/bin/qmake" ../zbwpos.pro -spec macx-clang CONFIG+=release

echo "Building..."
make -j$(sysctl -n hw.ncpu)

# Create app bundle
echo "Creating app bundle..."
cd ..
rm -rf "$INSTALL_DIR"
mkdir -p "$INSTALL_DIR"

APP_BUNDLE="$APP_NAME.app"

# Find built executable
if [ -f "$BUILD_DIR/src/app/$APP_NAME.app/Contents/MacOS/$APP_NAME" ]; then
    cp -R "$BUILD_DIR/src/app/$APP_NAME.app" "$INSTALL_DIR/"
else
    # Create app bundle structure manually
    mkdir -p "$INSTALL_DIR/$APP_BUNDLE/Contents/MacOS"
    mkdir -p "$INSTALL_DIR/$APP_BUNDLE/Contents/Resources"

    cp "$BUILD_DIR/src/app/$APP_NAME" "$INSTALL_DIR/$APP_BUNDLE/Contents/MacOS/"

    # Create Info.plist
    cat > "$INSTALL_DIR/$APP_BUNDLE/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>$APP_NAME</string>
    <key>CFBundleIdentifier</key>
    <string>com.zbw.pos</string>
    <key>CFBundleName</key>
    <string>ZBW POS</string>
    <key>CFBundleVersion</key>
    <string>$APP_VERSION</string>
    <key>CFBundleShortVersionString</key>
    <string>$APP_VERSION</string>
    <key>LSMinimumSystemVersion</key>
    <string>$MACOSX_DEPLOYMENT_TARGET</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSSupportsAutomaticGraphicsSwitching</key>
    <true/>
</dict>
</plist>
EOF
fi

# Deploy Qt frameworks
echo "Deploying Qt frameworks..."
"$QT_DIR/bin/macdeployqt" "$INSTALL_DIR/$APP_BUNDLE" -no-translations

# Codesign (optional, requires Apple Developer certificate)
if [ -n "$APPLE_DEVELOPER_ID" ]; then
    echo "Signing application..."
    codesign --deep --force --verify --verbose --sign "$APPLE_DEVELOPER_ID" "$INSTALL_DIR/$APP_BUNDLE"
fi

# Create DMG
echo "Creating DMG..."
DMG_NAME="${APP_NAME}-${APP_VERSION}-macos.dmg"
hdiutil create -volname "ZBW POS" -srcfolder "$INSTALL_DIR/$APP_BUNDLE" -ov -format UDZW "$DMG_NAME"

mv "$DMG_NAME" "$INSTALL_DIR/"

echo "========================================"
echo "Build complete: $INSTALL_DIR/$APP_BUNDLE"
echo "DMG: $INSTALL_DIR/$DMG_NAME"
echo "========================================"
