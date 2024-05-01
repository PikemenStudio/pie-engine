VULKAN_PATH=""

git submodule update --init --recursive
./vcpkg/bootstrap-vcpkg.sh -disableMetrics

export VULKAN_SDK=${VULKAN_PATH}

sudo apt install libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev pkg-config