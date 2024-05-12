cmake_minimum_required(VERSION 3.5.0)

# Set global flags

if (UNIX AND NOT APPLE)
    set (CMAKE_CXX_COMPILER g++-10)
endif (UNIX AND NOT APPLE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_STANDARD 20)

option(TESTS "Build tests (tests will compile modules)" OFF)

project(Engine VERSION 0.0.1 LANGUAGES C CXX)

# compile tests
add_subdirectory(src/modules/windows/src)
add_subdirectory(src/modules/graphics/src)
add_subdirectory(src/modules/sound/src)
set(ADAPTERS ${ADAPTERS}
        src/modules/windows/facades/facade.hpp
        src/modules/graphics/facades/facade.hpp
        src/modules/sound/facade/facade.hpp
)

add_subdirectory(tests)
add_subdirectory(game)

add_executable(main src/main.cpp ${ADAPTERS})

# link module
target_link_libraries(main PRIVATE windows)
include_directories(src/modules/windows/facades)

target_link_libraries(main PRIVATE graphics)
include_directories(src/modules/graphics/facades)

target_link_libraries(main PRIVATE sound)
include_directories(src/modules/sound/facade)

find_package(loguru CONFIG REQUIRED)
target_link_libraries(main PRIVATE loguru)

find_package(glm CONFIG REQUIRED)
target_link_libraries(main PRIVATE glm::glm)

# define path constants
target_compile_definitions(main PRIVATE VULKAN_SDK_PATH="$ENV{VULKAN_SDK}" PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
