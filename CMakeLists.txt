cmake_minimum_required(VERSION 3.16)
project(PolygonViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(PolygonViewer
    main.cpp
    mainwindow.cpp
    mainwindow.h
    polygon.cpp
    polygon.h
    renderwidget.cpp
    renderwidget.h
    booleanopsdialog.cpp
    booleanopsdialog.h
)

target_link_libraries(PolygonViewer
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
