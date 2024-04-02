cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL window.cpp)
set(HEADER_ALL window.h)

find_package(glm CONFIG REQUIRED)

add_library(windows ${SOURCE_ALL} ${HEADER_ALL})

# link libraries
# glm
target_link_libraries(windows PUBLIC glm::glm)