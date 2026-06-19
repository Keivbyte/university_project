cmake_minimum_required(VERSION 3.20)
project(3_year)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(3_year
        Game_/src/main.cpp
        Game_/src/platform/window.cpp
        Game_/src/platform/window.h
)

target_include_directories(3_year PRIVATE Game_/src)