cmake_minimum_required(VERSION 3.5.0)

# Set global flags
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_STANDARD 20)

project(Engine VERSION 0.0.1 LANGUAGES C CXX)

find_package(GTest REQUIRED)
find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(main src/main.cpp)

#add_executable(test_exe "Test/Test.cpp" ${CORE_ALL} ${UTILS_ALL})
#add_test(test test_exe)

# define path constants
target_compile_definitions(main PRIVATE VULKAN_SDK_PATH="$ENV{VULKAN_SDK}" PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

# test section with other executables
enable_testing()

# gtest
#target_link_libraries(test_exe PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

# vulkan
if (win32)
    target_link_directories(main PRIVATE $ENV{VULKAN_SDK}/Lib)
    target_link_libraries(main PRIVATE Vulkan-1)
endif (win32)

if (APPLE)
    target_link_libraries(main PRIVATE $ENV{VULKAN_SDK}/lib/libvulkan.dylib)
endif (APPLE)

# glfw
#target_link_libraries(test_exe PUBLIC glfw)
target_link_libraries(main PUBLIC glfw)

# glm
#target_link_libraries(test_exe PUBLIC glm::glm)
target_link_libraries(main PUBLIC glm::glm)
