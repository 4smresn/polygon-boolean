# 多边形布尔运算查看器

一个基于 Qt6 和 Clipper2 的多边形布尔运算可视化工具。

## 功能特性

- ✅ **多边形加载** - 支持从文本文件加载多边形点集
- ✅ **带孔多边形** - 支持 `#loop` 标记的多循环多边形（外轮廓 + 孔洞）
- ✅ **可视化渲染** - 实时渲染多边形，支持缩放和自动居中
- ✅ **模型管理** - 列表显示所有模型，支持显示/隐藏控制
- ✅ **高亮显示** - 点击列表项高亮显示对应多边形
- ✅ **布尔运算** - 支持并集、交集、差集运算
- ✅ **多结果支持** - 布尔运算可生成多个独立的结果多边形
- ✅ **追加加载** - 支持连续加载多个文件而不清空已有模型

## 技术栈

- **Qt 6** - GUI 框架
- **Clipper2** - 多边形布尔运算库
- **CMake** - 构建系统
- **C++17** - 编程语言

## 构建说明

### 依赖

- Qt 6 (Core, Widgets)
- CMake 3.16+
- C++17 编译器（GCC 7+, Clang 5+, MSVC 2017+）
- Clipper2 库（已包含在 third_party 目录）

### 编译步骤

#### Linux / macOS

```bash
# 1. 克隆仓库
git clone https://github.com/4smresn/polygon-boolean.git
cd polygon-boolean

# 2. 下载 Clipper2（如果 third_party 为空）
mkdir -p third_party
cd third_party
git clone https://github.com/AngusJohnson/Clipper2.git
cd ..

# 3. 构建并运行
./test.sh

# 或者手动构建
mkdir build
cd build
cmake ..
make
./PolygonViewer
```

#### Windows

```batch
# 1. 克隆仓库
git clone https://github.com/4smresn/polygon-boolean.git
cd polygon-boolean

# 2. 下载 Clipper2（如果 third_party 为空）
mkdir third_party
cd third_party
git clone https://github.com/AngusJohnson/Clipper2.git
cd ..

# 3. 构建并运行
test.bat

# 或者使用 CMake GUI / Visual Studio
mkdir build
cd build
cmake ..
cmake --build . --config Release
Release\PolygonViewer.exe
```

#### 快速构建脚本

- **Linux/macOS**: `./test.sh` (完整构建) 或 `./test.sh --run` (仅运行)
- **Windows**: `test.bat` (完整构建) 或 `test.bat --run` (仅运行)

## 文件格式

支持的点集文件格式：

```
# 简单多边形（单个轮廓）
0.0 0.0
1.0 0.0
1.0 1.0
0.0 1.0
0.0 0.0

# 带孔多边形（多个循环）
#loop
0.0 0.0
1.0 0.0
1.0 1.0
0.0 1.0
0.0 0.0
#loop
0.3 0.3
0.3 0.7
0.7 0.7
0.7 0.3
0.3 0.3
```

### 格式说明

- 每行两个浮点数，表示一个点的 x、y 坐标
- `#loop` 标记开始新的循环
- 第一个循环是外轮廓（逆时针）
- 后续循环是孔洞（顺时针）
- 以 `#` 或 `//` 开头的行为注释（会被忽略）

## 使用方法

1. **加载模型** - 点击"加载点集文件"按钮选择文件
2. **管理显示** - 使用复选框控制每个模型的显示/隐藏
3. **高亮模型** - 点击列表项高亮显示对应多边形
4. **布尔操作** - 点击"布尔操作"按钮，选择两个模型和操作类型
5. **查看结果** - 布尔运算结果会自动添加到模型列表

## 项目结构

```
.
├── main.cpp                    # 主程序入口
├── mainwindow.h/cpp            # 主窗口
├── polygon.h/cpp               # 多边形类
├── renderwidget.h/cpp          # 渲染控件
├── booleanopsdialog.h/cpp      # 布尔操作对话框
├── op/
│   ├── booleanops.h            # 布尔运算接口
│   └── booleanops.cpp          # 布尔运算实现（基于 Clipper2）
├── third_party/
│   └── Clipper2/               # Clipper2 库
└── CMakeLists.txt              # CMake 配置
```

## 截图

（TODO: 添加截图）

## 许可证

本项目使用 MIT 许可证。

Clipper2 使用 Boost Software License 1.0。

## 贡献

欢迎提交 Issue 和 Pull Request！

## 致谢

- [Clipper2](https://github.com/AngusJohnson/Clipper2) - 强大的多边形裁剪库
- [Qt](https://www.qt.io/) - 优秀的跨平台 GUI 框架
