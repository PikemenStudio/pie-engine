set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

# test section with other executables
enable_testing()

find_package(GTest REQUIRED)
find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(test_all "test_all.cpp"
        window_module.hpp
        graphic_module.hpp
        sound_module.hpp
        shader_loader_module.h)

add_test(test_all test_list_all_devices_exe)

target_link_libraries(test_all PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

target_link_libraries(test_all PRIVATE windows)
target_link_libraries(test_all PRIVATE graphics)
target_link_libraries(test_all PRIVATE sound)
target_link_libraries(test_all PRIVATE shader_loader)

include_directories(../src/modules)
