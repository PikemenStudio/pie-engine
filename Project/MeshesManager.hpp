//
// Created by Full-Hat on 19/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "Project/Mesh.hpp"

class MeshesManager {
public:
    MeshesManager();
    ~MeshesManager();
    void Consume(std::string type, std::vector<float> vertexData);
    void Finalize(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice);
    Memory::Buffer vertexBuffer;

    struct DataUnit {
        int offset;
        int size;
    };
    std::unordered_map<std::string, DataUnit> data;
private:
    int offset;
    vk::Device logicalDevice;
    std::vector<float> lump;
};
