set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-noise CONFIG REQUIRED)
find_package(unofficial-noiseutils CONFIG REQUIRED)

set(SFML_WARNINGS_AS_ERRORS OFF)

set(SFML_STATIC_LIBRARIES TRUE)

add_executable(game_exe
        src/main.cpp
        src/Tunnel.cpp
        src/Tunnel.h
        src/utils.h
        src/utils.cpp
        src/Game.cpp
        src/Game.h
        src/LightSource.cpp
        src/LightSource.h
        src/Background.cpp
        src/Background.h
        src/WorldWindow.cpp
        src/WorldWindow.h
        src/Player.cpp
        src/Player.h)

target_link_libraries(game_exe PRIVATE windows)
target_link_libraries(game_exe PRIVATE graphics)
target_link_libraries(game_exe PRIVATE sound)
target_link_libraries(game_exe PRIVATE unofficial::noise::noise-static)
target_link_libraries(game_exe PRIVATE unofficial::noiseutils::noiseutils-static)

if (APPLE)
    target_link_libraries(game_exe PRIVATE $ENV{SFML_PATH}/lib/libsfml-system.2.6.1.dylib
            $ENV{SFML_PATH}/lib/libsfml-graphics.2.6.1.dylib
            $ENV{SFML_PATH}/lib/libsfml-window.2.6.1.dylib)
    target_include_directories(game_exe PRIVATE $ENV{SFML_PATH}/include)
endif (APPLE)

if (UNIX AND NOT APPLE)
    find_package(SFML COMPONENTS graphics window system REQUIRED)
    target_link_libraries(game_exe PRIVATE sfml-system sfml-graphics sfml-window)
endif (UNIX AND NOT APPLE)

include_directories(../src/modules)
