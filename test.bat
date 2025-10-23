@echo off
REM Windows 构建脚本

REM 检查是否有 --run 参数
if "%1"=="--run" (
    cd build
    PolygonViewer.exe
    exit /b 0
)

REM 默认：重新构建并运行
if exist build rmdir /s /q build
mkdir build
cd build
cmake ..
cmake --build . --config Release
Release\PolygonViewer.exe
