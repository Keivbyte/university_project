cmake_minimum_required(VERSION 3.20)
project(3_year)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(game_math INTERFACE)
target_include_directories(game_math INTERFACE Game_/src)
target_compile_features(game_math INTERFACE cxx_std_20)

if(WIN32)

add_executable(3_year
        Game_/src/client/main.cpp
        Game_/src/client/platform/window.cpp
        Game_/src/client/platform/window.h
        Game_/src/client/platform/framebuffer.h
        Game_/src/client/platform/framebuffer.cpp
        Game_/src/client/platform/input.h
        Game_/src/client/platform/input.cpp
        Game_/src/shared/time/timer.h
        Game_/src/shared/time/timer.cpp
        Game_/src/shared/net/socket.h
        Game_/src/shared/net/protocol.h
        Game_/src/shared/serialization/byte_stream.h
        Game_/src/server/app/server_main.cpp
)

target_include_directories(3_year PRIVATE Game_/src)
target_compile_definitions(3_year PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(3_year PRIVATE game_math)
endif()

include(CTest)
if(BUILD_TESTING)
    foreach(mode IN ITEMS debug release)
        add_executable(math_tests_${mode} Game_/tests/math_tests.cpp)
        target_link_libraries(math_tests_${mode} PRIVATE game_math)
        if(mode STREQUAL "release")
            target_compile_definitions(math_tests_${mode} PRIVATE NDEBUG)
        endif()
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(math_tests_${mode} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            if(mode STREQUAL "debug")
                target_compile_options(math_tests_${mode} PRIVATE -UNDEBUG)
            else()
                target_compile_options(math_tests_${mode} PRIVATE -O2)
            endif()
        elseif(MSVC)
            target_compile_options(math_tests_${mode} PRIVATE /W4 /WX)
            if(mode STREQUAL "debug")
                target_compile_options(math_tests_${mode} PRIVATE /UNDEBUG)
            endif()
        endif()
        add_test(NAME math_${mode} COMMAND math_tests_${mode})
    endforeach()
endif()
