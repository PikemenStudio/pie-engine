# Pie Engine
## Description

## Prerequisites
1. Download SDK for your system from https://vulkan.lunarg.com/sdk/home
2. Clone repo by
   ```
   git clone git@github.com:PikemenStudio/pie-engine.git && cd pie-engine
   ```
3. run
   ```
   git submodule update --init --recursive
   ```
4. Setup vcpkg
   ```
   ./vcpkg/bootstrap-vcpkg.sh -disableMetrics
   ```
## Build Instructions
### Apple
Set VULKAN_SDK env for your system:
```sh
export VULKAN_SDK=~/VulkanSDK/1.3.268.1/macOS/
```
### Windows
Not tested yet
## Usage
