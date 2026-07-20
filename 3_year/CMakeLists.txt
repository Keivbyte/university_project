cmake_minimum_required(VERSION 3.20)
project(3_year)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)

add_executable(3_year
        Game_/src/client/main.cpp
        Game_/src/client/platform/window.cpp
        Game_/src/client/platform/window.h
        Game_/src/client/platform/framebuffer.h
        Game_/src/client/platform/framebuffer.cpp
        Game_/src/shared/time/timer.h
        Game_/src/shared/time/timer.cpp
        Game_/src/shared/net/socket.h
        Game_/src/shared/net/protocol.h
        Game_/src/shared/serialization/byte_stream.h
        Game_/src/server/app/server_main.cpp
)

target_include_directories(3_year PRIVATE Game_/src)