#!/bin/bash
# ZBW POS Master Build Script
# Builds and packages for all platforms

set -e

APP_NAME="zbwpos"
VERSION="${VERSION:-1.0.0}"

echo "========================================"
echo "ZBW POS Multi-Platform Build"
echo "Version: $VERSION"
echo "========================================"

# Create package directory
mkdir -p package

# Detect current platform and build
OS=$(uname -s)
case "$OS" in
    Linux*)
        echo "Building for Linux..."
        bash scripts/build-linux.sh
        ;;
    Darwin*)
        echo "Building for macOS..."
        bash scripts/build-macos.sh
        ;;
    MINGW*|CYGWIN*|MSYS*)
        echo "Building for Windows..."
        scripts/build-windows.bat
        ;;
    *)
        echo "Unknown platform: $OS"
        exit 1
        ;;
esac

echo ""
echo "========================================"
echo "Build Complete!"
echo "Packages in ./package/"
echo "========================================"
ls -la package/ 2>/dev/null || echo "No packages created"
