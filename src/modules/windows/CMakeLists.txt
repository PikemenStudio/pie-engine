cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL src/window.cpp)
set(HEADER_ALL src/window.h connectors/fabric.hpp)

find_package(glm CONFIG REQUIRED)

include_directories(../connectors)

add_library(windows ${SOURCE_ALL} ${HEADER_ALL})

# link libraries
# glm
target_link_libraries(windows PUBLIC glm::glm)