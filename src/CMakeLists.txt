cmake_minimum_required(VERSION 3.16)

project(CGame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    main.cpp
    CGame.cpp
)

set(HEADERS
    CGame.h
)

set(FORMS
    CGame.ui
)

set(RESOURCES
    CGame.qrc
)

add_executable(CGame
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

target_link_libraries(CGame PRIVATE Qt6::Widgets)
