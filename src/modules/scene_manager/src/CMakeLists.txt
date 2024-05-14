cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL facade.cpp SceneManager.cpp)
set(HEADER_ALL SceneManager.hpp)

find_package(loguru CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(scene_manager ${SOURCE_ALL} ${HEADER_ALL})

target_link_libraries(scene_manager PRIVATE loguru)
target_link_libraries(scene_manager PUBLIC glm::glm)
