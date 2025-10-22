cmake_minimum_required(VERSION 3.16)
project(PolygonViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 添加 Clipper2 头文件路径
include_directories(${CMAKE_SOURCE_DIR}/third_party/Clipper2/CPP/Clipper2Lib/include)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    polygon.cpp
    polygon.h
    renderwidget.cpp
    renderwidget.h
    booleanopsdialog.cpp
    booleanopsdialog.h
    op/booleanops.cpp
    op/booleanops.h
    # 添加 Clipper2 源文件
    third_party/Clipper2/CPP/Clipper2Lib/src/clipper.engine.cpp
    third_party/Clipper2/CPP/Clipper2Lib/src/clipper.offset.cpp
    third_party/Clipper2/CPP/Clipper2Lib/src/clipper.rectclip.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} 
    Qt6::Core
    Qt6::Widgets
)
