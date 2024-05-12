set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(game_exe main.cpp)

target_link_libraries(game_exe PRIVATE windows)
target_link_libraries(game_exe PRIVATE graphics)
target_link_libraries(game_exe PRIVATE sound)

include_directories(../src/modules)
