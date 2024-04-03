# Pie Engine
## Description

## Prerequisites
1. Download SDK for your system from https://vulkan.lunarg.com/sdk/home or use default package manager on Linux Systems (check build instructions)
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
### Linux
Use default package manager to download SDK
```
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.280-jammy.list https://packages.lunarg.com/vulkan/1.3.280/lunarg-vulkan-1.3.280-jammy.list
sudo apt update
sudo apt install vulkan-sdk
```
and run
```
mkdir build || cd build && cmake ../ && make
```
Instructions how to add other package version you can find on Vulkan SDK page

Otherwize create an issue
## Contributing
1. Create new branch from main with any name
2. Work in it :)
3. Create pull request to merge updates to main with SQUASH COMMITS option and the name of the commit according rule: "[brief] description" (like "[sound] fix sound; implement spaceship")
4. Check for all actions (format, tidy, tests)
5. Wait for merging
6. Sleep well

## Usage
