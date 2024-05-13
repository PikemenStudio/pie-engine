set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-noise CONFIG REQUIRED)
find_package(unofficial-noiseutils CONFIG REQUIRED)

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS graphics window system REQUIRED)

add_executable(game_exe main.cpp
        Floor.cpp
        Floor.h
        utils.h
        utils.cpp
        Game.cpp
        Game.h)

target_link_libraries(game_exe PRIVATE windows)
target_link_libraries(game_exe PRIVATE graphics)
target_link_libraries(game_exe PRIVATE sound)
target_link_libraries(game_exe PRIVATE unofficial::noise::noise-static)
target_link_libraries(game_exe PRIVATE unofficial::noiseutils::noiseutils-static)
target_link_libraries(game_exe PRIVATE sfml-system sfml-graphics sfml-window)

include_directories(../src/modules)
