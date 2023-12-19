//
// Created by Full-Hat on 19/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "Core/Memory.hpp"

class Mesh {
public:
    enum Types {
        eTriangle,
        eSquare,
    };

    static vk::VertexInputBindingDescription GetPosColorBindingDescription();
    static std::array<vk::VertexInputAttributeDescription,2> GetPosColorAttributeDescriptions();

    Memory::Buffer vertexBuffer;
};

class Triangle : public Mesh {
public:
    Triangle(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice);
    ~Triangle();

protected:
    vk::Device logicalDevice;
};
