cmake_minimum_required(VERSION 3.5.0)

# Set global flags

if (UNIX AND NOT APPLE)
    set (CMAKE_CXX_COMPILER g++-10)
endif (UNIX AND NOT APPLE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_STANDARD 20)

project(Game VERSION 0.0.1 LANGUAGES C CXX)

add_executable(game main.cpp)

