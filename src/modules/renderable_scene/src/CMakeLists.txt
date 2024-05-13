cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL facade.cpp RenderableScene.cpp)
set(HEADER_ALL RenderableScene.hpp)

find_package(loguru CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(renderable_scene ${SOURCE_ALL} ${HEADER_ALL})

target_link_libraries(renderable_scene PRIVATE loguru)
target_link_libraries(renderable_scene PUBLIC glm::glm)