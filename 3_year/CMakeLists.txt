cmake_minimum_required(VERSION 3.20)
project(3_year)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)

add_executable(3_year
        Game_/src/main.cpp
        Game_/src/platform/window.cpp
        Game_/src/platform/window.h
        Game_/src/platform/framebuffer.h
        Game_/src/platform/framebuffer.cpp
)

target_include_directories(3_year PRIVATE Game_/src)