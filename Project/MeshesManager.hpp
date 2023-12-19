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
    void Consume(Mesh::Types type, std::vector<float> vertexData);
    void Finalize(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice);
    Memory::Buffer vertexBuffer;
    std::unordered_map<Mesh::Types, int> offsets;
    std::unordered_map<Mesh::Types, int> sizes;
private:
    int offset;
    vk::Device logicalDevice;
    std::vector<float> lump;
};
