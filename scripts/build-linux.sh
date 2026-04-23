#!/bin/bash
# ZBW POS Linux Build Script
# Requires: Qt 5.15+ or Qt6, GCC/Clang, make

set -e

APP_NAME="zbwpos"
APP_VERSION="1.0.0"
BUILD_DIR="build-linux"
INSTALL_DIR="install"
PACKAGE_DIR="package"

# Detect Qt installation
detect_qt() {
    if [ -n "$QT_DIR" ]; then
        return
    fi

    # Common Qt installation paths
    local paths=(
        "/opt/Qt/5.15.2/gcc_64"
        "/opt/Qt/6.5.0/gcc_64"
        "/usr/lib/qt5"
        "/usr/lib/qt6"
        "/usr/lib/x86_64-linux-gnu/qt5"
        "/usr/lib/x86_64-linux-gnu/qt6"
        "$HOME/Qt/5.15.2/gcc_64"
        "$HOME/Qt/6.5.0/gcc_64"
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

echo "========================================"
echo "ZBW POS Linux Build"
echo "Qt: $QT_DIR"
echo "========================================"

# Clean previous build
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Build
cd "$BUILD_DIR"
echo "Running qmake..."
"$QT_DIR/bin/qmake" ../zbwpos.pro -spec linux-g++ CONFIG+=release

echo "Building..."
make -j$(nproc)

# Install
echo "Installing to $INSTALL_DIR..."
cd ..
rm -rf "$INSTALL_DIR"
mkdir -p "$INSTALL_DIR/bin"

cp "$BUILD_DIR/src/app/$APP_NAME" "$INSTALL_DIR/bin/" 2>/dev/null || \
cp "$BUILD_DIR/src/app/release/$APP_NAME" "$INSTALL_DIR/bin/"

# Deploy Qt dependencies
echo "Deploying Qt dependencies..."
cd "$INSTALL_DIR/bin"
"$QT_DIR/bin/linuxdeployqt" "$APP_NAME" -bundle -no-translations 2>/dev/null || {
    echo "linuxdeployqt not found, skipping deployment"
    echo "Install linuxdeployqt for automatic dependency bundling"
}

cd ../..

# Create AppImage if possible
if [ -x "$(command -v appimagetool)" ]; then
    echo "Creating AppImage..."
    mkdir -p "$PACKAGE_DIR/AppImage/usr/bin"
    cp "$INSTALL_DIR/bin/$APP_NAME" "$PACKAGE_DIR/AppImage/usr/bin/"

    # Create desktop file
    cat > "$PACKAGE_DIR/AppImage/$APP_NAME.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=ZBW POS
Exec=$APP_NAME
Icon=$APP_NAME
Categories=Office;Finance;
EOF

    appimagetool "$PACKAGE_DIR/AppImage" "$PACKAGE_DIR/$APP_NAME-$APP_VERSION-x86_64.AppImage"
fi

# Create .deb package
if [ -x "$(command -v dpkg-deb)" ]; then
    echo "Creating .deb package..."
    DEB_DIR="$PACKAGE_DIR/deb"
    mkdir -p "$DEB_DIR/DEBIAN"
    mkdir -p "$DEB_DIR/usr/bin"
    mkdir -p "$DEB_DIR/usr/share/applications"
    mkdir -p "$DEB_DIR/usr/share/pixmaps"

    cp "$INSTALL_DIR/bin/$APP_NAME" "$DEB_DIR/usr/bin/"

    cat > "$DEB_DIR/usr/share/applications/$APP_NAME.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=ZBW POS
Exec=$APP_NAME
Icon=$APP_NAME
Categories=Office;Finance;
EOF

    cat > "$DEB_DIR/DEBIAN/control" <<EOF
Package: $APP_NAME
Version: $APP_VERSION
Section: office
Priority: optional
Architecture: amd64
Maintainer: ZBW Team <team@zbw.com>
Description: ZBW POS - Smart Cashier System
 Cross-platform retail POS cashier client with offline support.
EOF

    dpkg-deb --build "$DEB_DIR" "$PACKAGE_DIR/${APP_NAME}_${APP_VERSION}_amd64.deb"
fi

echo "========================================"
echo "Build complete: $INSTALL_DIR/bin/$APP_NAME"
echo "========================================"
