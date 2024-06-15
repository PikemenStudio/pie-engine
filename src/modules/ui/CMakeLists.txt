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

set(SOURCE_ALL UI.cpp
        ${IMGUI_BACKENDS_SOURCE}
        ${IMGUI_SOURCE})

include_directories(${IMGUI_PATH})

set(HEADER_ALL UI.hpp)

find_package(imgui CONFIG REQUIRED)

add_library(UI STATIC ${SOURCE_ALL} ${HEADER_ALL})

target_include_directories(graphics PRIVATE ${IMGUI_PATH})
