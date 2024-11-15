cmake_minimum_required(VERSION 3.14)

project(
    SnakeGame2
    VERSION 1.0.0
    DESCRIPTION "Snake Game with angle transformations"
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(LINK_FLAGS "-mwindows -mconsole -Wl,-subsystem,console")

set(sources
    src/snake.h
    src/snake.cpp
    src/main.cpp
    resources.qrc
)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

add_executable(SnakeGame2 ${sources})

target_link_libraries(SnakeGame2 PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui)

if (WIN32)
    set_property(TARGET SnakeGame2 PROPERTY WIN32_EXECUTABLE true)
elseif (APPLE)
    set(CMAKE_MACOSX_BUNDLE TRUE)
    set_target_properties(SnakeGame2 PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist)
endif()
if (UNIX AND NOT APPLE)
    install(TARGETS SnakeGame2 DESTINATION /usr/local/bin)
endif()