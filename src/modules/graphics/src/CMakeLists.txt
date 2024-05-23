cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

find_path(IMGUI_PATH imgui.h)
message("Imgui path " ${IMGUI_PATH})
set(IMGUI_BACKENDS_PATH /Users/fullhat/Documents/GitHub/pie-engine/vcpkg/buildtrees/imgui/src/76b6bb5605-ca10468178.clean/backends)
message("Imgui path " ${IMGUI_BACKENDS_PATH})

file(GLOB IMGUI_BACKENDS_SOURCE "${IMGUI_BACKENDS_PATH}/../*.cpp" "${IMGUI_BACKENDS_PATH}/../*.h")
message("Source " ${IMGUI_BACKENDS_SOURCE})
set(IMGUI_SOURCE ${IMGUI_BACKENDS_PATH}/imgui_impl_glfw.cpp ${IMGUI_BACKENDS_PATH}/imgui_impl_vulkan.cpp)
message("Source " ${IMGUI_SOURCE})

# source files
set(SOURCE_ALL VkInstance.cpp VkPhysicalDevice.cpp
        GraphicEngine.cpp
        facade.cpp
        VkPipeline.cpp
        VkTexture.cpp
        ${IMGUI_BACKENDS_SOURCE}
        ${IMGUI_SOURCE})
set(HEADER_ALL VulkanInstance.hpp VulkanPhysicalDevice.hpp
        GraphicEngine.hpp ObjectsMemory.hpp VulkanPipeline.h VkTexture.hpp)

find_package(VulkanHeaders CONFIG)
find_package(glfw3 CONFIG REQUIRED)
find_package(loguru CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_library(graphics ${SOURCE_ALL} ${HEADER_ALL})

target_include_directories(graphics PRIVATE ${IMGUI_PATH})
target_include_directories(graphics PRIVATE ${Stb_INCLUDE_DIR})

target_link_libraries(graphics PRIVATE loguru)
target_link_libraries(graphics PRIVATE scene_manager)

# link libraries
# vulkan
if (win32)
    target_link_directories(graphics PRIVATE $ENV{VULKAN_SDK}/Lib)
    target_link_libraries(graphics PRIVATE Vulkan-1)
endif (win32)

if (APPLE)
    target_link_libraries(graphics PRIVATE $ENV{VULKAN_SDK}/lib/libvulkan.dylib)
endif (APPLE)

if (UNIX AND NOT APPLE)
    find_package(Vulkan REQUIRED)
    target_link_libraries(graphics PRIVATE Vulkan::Vulkan)
endif (UNIX AND NOT APPLE)

# glfw
target_link_libraries(graphics PUBLIC glfw)

#target_link_libraries(graphics PRIVATE imgui)