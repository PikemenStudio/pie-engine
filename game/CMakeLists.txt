set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-noise CONFIG REQUIRED)
find_package(unofficial-noiseutils CONFIG REQUIRED)

add_executable(game_exe main.cpp)

target_link_libraries(game_exe PRIVATE windows)
target_link_libraries(game_exe PRIVATE graphics)
target_link_libraries(game_exe PRIVATE sound)
target_link_libraries(game_exe PRIVATE unofficial::noise::noise-static)
target_link_libraries(game_exe PRIVATE unofficial::noiseutils::noiseutils-static)

include_directories(../src/modules)
