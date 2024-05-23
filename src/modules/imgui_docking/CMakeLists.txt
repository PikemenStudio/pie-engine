cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp backends/imgui_impl_glfw.cpp backends/imgui_impl_vulkan.cpp)
set(HEADER_ALL)

find_package(loguru CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG)

add_library(imgui ${SOURCE_ALL} ${HEADER_ALL})

target_include_directories(imgui PRIVATE /Users/fullhat/Documents/GitHub/pie-engine/src/modules/imgui_docking)

target_link_libraries(imgui PRIVATE loguru)
target_link_libraries(imgui PUBLIC glfw)

if (win32)
    target_link_directories(imgui PRIVATE $ENV{VULKAN_SDK}/Lib)
    target_link_libraries(imgui PRIVATE Vulkan-1)
endif (win32)

if (APPLE)
    target_link_libraries(imgui PRIVATE $ENV{VULKAN_SDK}/lib/libvulkan.dylib)
endif (APPLE)

if (UNIX AND NOT APPLE)
    find_package(Vulkan REQUIRED)
    target_link_libraries(imgui PRIVATE Vulkan::Vulkan)
endif (UNIX AND NOT APPLE)
