cmake_minimum_required(VERSION 3.5.0)

# Set global flags
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_CXX_COMPILE_FEATURES 20)
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(Engine VERSION 0.0.1 LANGUAGES C CXX)

# executable file
# add main library file for engine
add_library(Engine Engine.cpp)

# test section with other executables
enable_testing()

find_package(GTest CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
