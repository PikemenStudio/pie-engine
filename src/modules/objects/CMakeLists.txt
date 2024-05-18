cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL Triangle.cpp Square.cpp)
set(HEADER_ALL AllObjects.hpp BaseObject.hpp Triangle.hpp Square.hpp)

find_package(loguru CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(objects ${SOURCE_ALL} ${HEADER_ALL})

target_link_libraries(objects PRIVATE loguru)
target_link_libraries(objects PUBLIC glm::glm)
