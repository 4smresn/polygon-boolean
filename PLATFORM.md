# 跨平台兼容性说明

## 支持的平台

本项目支持以下操作系统：

| 操作系统 | 架构 | 测试状态 | 备注 |
|---------|------|---------|------|
| **Linux** | x86_64 | ✅ 已测试 | Ubuntu 20.04+, Fedora 35+ |
| **Windows** | x86_64 | ✅ 理论支持 | Windows 10/11 |
| **macOS** | x86_64, arm64 | ✅ 理论支持 | macOS 10.15+ |

## 构建工具要求

### Linux

```bash
# Ubuntu/Debian
sudo apt install build-essential cmake qt6-base-dev

# Fedora/RHEL
sudo dnf install gcc-c++ cmake qt6-qtbase-devel

# Arch
sudo pacman -S base-devel cmake qt6-base
```

### Windows

- **Visual Studio 2019/2022** (带 C++ 工作负载)
- **CMake** 3.16+
- **Qt 6** ([下载地址](https://www.qt.io/download))

推荐使用 Qt 在线安装器，选择 MSVC 2019 64-bit 组件。

### macOS

```bash
# 使用 Homebrew 安装
brew install cmake qt@6

# 设置 Qt 环境变量
export Qt6_DIR=$(brew --prefix qt@6)/lib/cmake/Qt6
```

或者从 Qt 官网下载安装包。

## 平台特定说明

### Windows

1. **路径问题**
   - 使用 CMake 会自动处理路径分隔符
   - 避免在代码中硬编码 `/` 或 `\`

2. **字符编码**
   - 已在 CMakeLists.txt 中设置 Unicode 支持
   - 文件名和路径自动处理中文等字符

3. **可执行文件**
   - 默认生成 `.exe` 文件
   - 可以设置 `WIN32_EXECUTABLE` 隐藏控制台窗口

4. **部署**
   ```batch
   # 使用 windeployqt 打包依赖
   cd build/Release
   windeployqt PolygonViewer.exe
   ```

### macOS

1. **应用包**
   - 可以设置 `MACOSX_BUNDLE` 生成 `.app` 包
   - 使用 macdeployqt 打包依赖

2. **签名和公证**
   - 如果要分发，需要 Apple Developer 账号
   - 运行以下命令签名：
   ```bash
   codesign --sign "Developer ID" PolygonViewer.app
   ```

3. **部署**
   ```bash
   # 使用 macdeployqt 打包依赖
   macdeployqt PolygonViewer.app
   ```

### Linux

1. **依赖分发**
   - 使用 AppImage 打包
   - 或使用系统包管理器分发

2. **桌面集成**
   - 创建 `.desktop` 文件
   - 安装到 `/usr/share/applications/`

## 已知兼容性问题

### 无已知问题 ✅

目前项目使用的所有库和 API 都是跨平台的：

- ✅ Qt 6 - 完全跨平台
- ✅ C++17 标准库 - 跨平台
- ✅ CMake - 跨平台构建
- ✅ Clipper2 - 纯 C++ 实现

### 潜在问题

1. **浮点精度**
   - 不同平台的浮点运算可能有微小差异
   - 已使用 `eps = 1e-10` 处理

2. **文件路径**
   - 使用 Qt 的 `QDir` 和 `QFileInfo` 自动处理
   - 避免硬编码路径分隔符

3. **字体渲染**
   - 不同平台字体可能略有差异
   - 已设置统一字体大小

## 测试建议

在部署到新平台前，建议测试以下功能：

- [ ] 加载点集文件
- [ ] 渲染带孔多边形
- [ ] 执行布尔运算（并集、交集、差集）
- [ ] 窗口缩放和调整
- [ ] 文件路径包含非 ASCII 字符
- [ ] 大量模型加载

## CI/CD 支持

可以使用 GitHub Actions 进行多平台自动构建：

```yaml
# .github/workflows/build.yml
name: Build

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    runs-on: ${{ matrix.os }}
    
    steps:
      - uses: actions/checkout@v3
      - name: Install Qt
        uses: jurplel/install-qt-action@v3
        with:
          version: '6.5.0'
      - name: Build
        run: |
          mkdir build
          cd build
          cmake ..
          cmake --build . --config Release
```

## 技术支持

如果在特定平台遇到问题，请在 GitHub Issues 中报告，包含以下信息：

- 操作系统和版本
- Qt 版本
- CMake 版本
- 编译器版本
- 完整错误日志
