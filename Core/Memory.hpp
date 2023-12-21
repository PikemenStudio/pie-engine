//
// Created by Full-Hat on 19/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include <cstdint>

class Memory {
public:
    struct Buffer {
        vk::Buffer buffer;
        vk::DeviceMemory bufferMemory;
    };

    struct BufferInputChunk {
        size_t size = 0;
        vk::BufferUsageFlags usage {};
        vk::Device logicalDevice;
        vk::PhysicalDevice physicalDevice;
        vk::MemoryPropertyFlags memoryProperties;
    };

    static uint32_t findMemoryTypeIndex(
            vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices,
            vk::MemoryPropertyFlags requestedProperties);

    static void allocateBufferMemory(Buffer& buffer, const BufferInputChunk& input);

    static Buffer createBuffer(BufferInputChunk input);

    static void copyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer commandBuffer);
};
