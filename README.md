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
Build project
```
mkdir build || cd build && cmake ../ && make
```
### Windows
Not tested yet
## Contributing
1. Create new branch from main with name "[breaf] description" (like "[sound] sound module")
2. Work in it :)
3. Create pull request to merge updates to main with SQUASH COMMITS option
4. Check for all actions (format, tidy, tests)
5. Wait for merging
6. Sleep well

## Usage
