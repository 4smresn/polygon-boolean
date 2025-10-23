cmake_minimum_required(VERSION 3.16)
project(PolygonViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 支持 Windows 下的 Unicode
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# macOS 特定设置
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")
endif()

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

# Windows 下不显示控制台窗口（可选）
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# macOS 下创建应用包（可选）
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

target_link_libraries(${PROJECT_NAME} 
    Qt6::Core
    Qt6::Widgets
)
