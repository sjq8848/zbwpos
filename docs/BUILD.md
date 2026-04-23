# ZBW POS 构建指南

## 构建要求

### Windows
- **Qt 5.15.2** (MSVC 2019 64-bit) 或 **Qt 6.x**
- Visual Studio 2019/2022 (MSVC 编译器)
- NSIS (可选，用于创建安装包)

### macOS
- **Qt 5.15.2** (clang_64) 或 **Qt 6.x**
- Xcode Command Line Tools
- macOS 10.14+

### Linux
- **Qt 5.15+** 或 **Qt 6.x**
- GCC 9+ 或 Clang 10+
- 依赖: `qt5-default qtbase5-dev qt5-qmake libqt5sql5-sqlite`

### Android
- **Qt 5.15.2** (android_arm64_v8a)
- Android SDK (API 21+)
- Android NDK (r21+)
- JDK 11+

## 快速开始

### 本地构建

```bash
# 设置环境变量
export QT_DIR=/path/to/qt  # 或 C:\Qt\5.15.2\msvc2019_64 (Windows)
export VERSION=1.0.0

# 执行构建
./scripts/build-all.sh      # Linux/macOS
scripts\build-windows.bat    # Windows
```

### 单平台构建

```bash
# Windows
scripts\build-windows.bat

# macOS
bash scripts/build-macos.sh

# Linux
bash scripts/build-linux.sh

# Android
bash scripts/build-android.sh
```

### 设置 Android 环境

```bash
# 设置环境变量
export ANDROID_SDK=$HOME/Android/sdk
export ANDROID_NDK=$ANDROID_SDK/ndk/23.1.7779620
export QT_ANDROID=/path/to/qt/android

# 构建
bash scripts/build-android.sh
```

## GitHub Actions 自动发布

项目配置了 GitHub Actions CI/CD，支持自动跨平台构建：

### 触发方式

1. **标签发布**: 推送 `v*` 标签自动构建所有平台
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```

2. **手动触发**: 在 GitHub Actions 页面手动运行 workflow

### 输出文件

| 平台 | 文件格式 |
|------|----------|
| Windows | `.exe` (安装包) + `.zip` (便携版) |
| macOS | `.dmg` |
| Linux | `.tar.gz` |
| Android | `.apk` |

## 项目结构

```
zbwpos/
├── src/               # 源代码
├── scripts/           # 构建脚本
│   ├── build-windows.bat
│   ├── build-macos.sh
│   ├── build-linux.sh
│   ├── build-android.sh
│   └── installer-windows.nsi
├── .github/workflows/ # CI/CD 配置
└── docs/              # 文档
```

## Qt 安装

### 在线安装器

从 [Qt 官网](https://www.qt.io/download) 下载在线安装器，选择：
- Qt 5.15.2 或 Qt 6.x
- 对应平台的编译器版本

### 建议组件

**Windows:**
- MSVC 2019 64-bit
- Qt Charts (可选)

**macOS:**
- macOS (clang_64)

**Linux:**
- Desktop (gcc_64)

**Android:**
- Android ARM64 v8a
- Android ARMv7 (可选)

## 常见问题

### Q: qmake: command not found
设置 `QT_DIR` 环境变量或将 Qt bin 目录添加到 PATH。

### Q: Windows 构建失败 - 找不到 MSVC
安装 Visual Studio Build Tools 或完整版 Visual Studio。
运行脚本会自动调用 `vcvars64.bat` 设置环境。

### Q: Android 构建失败
确保 `ANDROID_SDK` 和 `ANDROID_NDK` 环境变量正确指向 SDK/NDK 目录。

### Q: macOS 代码签名
设置 `APPLE_DEVELOPER_ID` 环境变量为企业证书标识。

## 版本号管理

项目版本由以下位置决定：
- `common.pri` 中的 `VERSION` 变量
- 构建脚本的 `APP_VERSION` 变量
- GitHub Actions 的 `APP_VERSION` 环境变量

修改版本时需同步更新。
