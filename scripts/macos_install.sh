VULKAN_PATH=~/VulkanSDK/$(ls -tr ~/VulkanSDK | tail -n 1)/macOS/

echo \[1/5\] Init files
git submodule update --init --recursive
./vcpkg/bootstrap-vcpkg.sh -disableMetrics

echo \[2/5\] Install dependencies
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"


brew install openal-soft
OPENAL_SOFT_PATH=$(brew --prefix openal-soft)
echo "OPENAL_PATH: $OPENAL_SOFT_PATH"

echo \[3/5\] export vairables
echo "VULKAN_PATH: $VULKAN_PATH"
export VULKAN_SDK=${VULKAN_PATH}
echo "OPENAL_PATH: $OPENAL_SOFT_PATH"
export OPENAL_PATH=${OPENAL_SOFT_PATH}

echo \[4/5\] Drink coffee
echo \[5/5\] Drink tea