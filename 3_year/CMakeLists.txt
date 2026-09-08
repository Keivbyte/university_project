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
        Game_/src/client/render/renderer.cpp
        Game_/src/client/render/renderer.h
        Game_/src/client/render/depth_buffer.cpp
        Game_/src/client/render/depth_buffer.h
        Game_/src/client/render/projection.cpp
        Game_/src/client/render/projection.h
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
        if(WIN32)
            foreach(render_test IN ITEMS rasterizer pipeline)
            add_executable(${render_test}_tests_${mode}
                Game_/tests/${render_test}_tests.cpp
                Game_/src/client/render/renderer.cpp
                Game_/src/client/render/depth_buffer.cpp
                Game_/src/client/render/projection.cpp
                Game_/src/client/platform/framebuffer.cpp)
            target_link_libraries(${render_test}_tests_${mode} PRIVATE game_math)
            target_compile_definitions(${render_test}_tests_${mode} PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
            if(mode STREQUAL "release")
                target_compile_definitions(${render_test}_tests_${mode} PRIVATE NDEBUG)
            endif()
            # Match the math test targets' strict warnings and assertion mode.
            get_target_property(test_options math_tests_${mode} COMPILE_OPTIONS)
            if(test_options)
                target_compile_options(${render_test}_tests_${mode} PRIVATE ${test_options})
            endif()
            add_test(NAME ${render_test}_${mode} COMMAND ${render_test}_tests_${mode})
            endforeach()
        endif()
    endforeach()
endif()
