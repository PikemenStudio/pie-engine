cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL InputManager.cpp)
set(HEADER_ALL InputManager.hpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(loguru CONFIG REQUIRED)

add_library(input_manager ${SOURCE_ALL} ${HEADER_ALL})

# glfw
target_link_libraries(input_manager PUBLIC glfw)