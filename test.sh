#!/bin/bash

# 检查是否有 --run 参数
if [ "$1" = "--run" ]; then
    ./build/PolygonViewer
    exit 0
fi

# 默认：重新构建并运行
rm -rf build
mkdir build
cd build
cmake ..
make
./PolygonViewer