//
// Created by Full-Hat on 19/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "Core/Memory.hpp"

class Mesh {
public:
    static vk::VertexInputBindingDescription GetPosColorBindingDescription();
    static std::array<vk::VertexInputAttributeDescription,2> GetPosColorAttributeDescriptions();
};
