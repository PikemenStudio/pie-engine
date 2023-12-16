cmake_minimum_required(VERSION 3.5.0)

# Set global flags
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_STANDARD 20)

project(Engine VERSION 0.0.1 LANGUAGES C CXX)

include_directories(./)

set(UTILS_SOURCE )
set(UTILS_HEADERS 
		"Utils/UtilsMacros.hpp")

set(CORE_SOURCE 
		"Core/Engine.cpp"
		"Core/Window/Window.cpp"
		Core/Instance.cpp
		Core/Instance.hpp)
set(CORE_HEADERS 
		"Core/Engine.hpp"
		"Core/Window/Window.hpp"
		Core/Instance.cpp
		Core/Instance.hpp)

set(UTILS_ALL ${UTILS_SOURCE} ${UTILS_HEADERS})
set(CORE_ALL ${CORE_SOURCE} ${CORE_HEADERS})

find_package(GTest REQUIRED)
find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(main "main.cpp" ${CORE_ALL} ${UTILS_ALL}
		Core/Instance.cpp
		Core/Instance.hpp
        Core/DebugMessanger.hpp
		Core/DebugMessanger.cpp
		Core/Gpu.cpp
		Core/Gpu.hpp
		Core/Window/VkWindow.cpp
		Core/Window/VkWindow.hpp
        Utils/VkLog.hpp)

add_executable(test_exe "Test/Test.cpp" ${CORE_ALL} ${UTILS_ALL})
add_test(test test_exe)

# test section with other executables
enable_testing()

# gtest
target_link_libraries(test_exe PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

# vulkan
if (win32)
	target_link_directories(main PRIVATE $ENV{VULKAN_SDK}/Lib)
	target_link_libraries(main PRIVATE Vulkan-1)
endif (win32)

if (APPLE)
	target_link_libraries(main PRIVATE /Users/full-hat/VulkanSDK/1.3.268.1/macOS/lib/libvulkan.dylib)
endif (APPLE)

# glfw
target_link_libraries(test_exe PUBLIC glfw)
target_link_libraries(main PUBLIC glfw)

# glm
target_link_libraries(test_exe PUBLIC glm::glm)
target_link_libraries(main PUBLIC glm::glm)
