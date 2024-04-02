set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

# test section with other executables
enable_testing()

find_package(GTest REQUIRED)
find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(test_list_all_devices_exe "test_list_all_devices.cpp")
add_test(test_list_all_devices test_list_all_devices_exe)

target_link_libraries(test_list_all_devices_exe PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

# vulkan
if (win32)
    link_directories($ENV{VULKAN_SDK}/Lib)
    link_libraries(Vulkan-1)
endif (win32)

if (APPLE)
    target_link_libraries(test_list_all_devices_exe PRIVATE $ENV{VULKAN_SDK}/lib/libvulkan.dylib)
endif (APPLE)

if (UNIX AND NOT APPLE)
    find_package(Vulkan REQUIRED)
    link_libraries(Vulkan::Vulkan)
endif (UNIX AND NOT APPLE)
