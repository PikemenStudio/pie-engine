//
// Created by Full-Hat on 19/12/2023.
//

#include "Memory.hpp"

uint32_t Memory::findMemoryTypeIndex(
        vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices,
        vk::MemoryPropertyFlags requestedProperties) {
    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {

        //bit i of supportedMemoryIndices is set if that memory type is supported by the device
        bool supported{ static_cast<bool>(supportedMemoryIndices & (1 << i)) };

        //propertyFlags holds all the memory properties supported by this memory type
        bool sufficient{ (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties };

        if (supported && sufficient) {
            return i;
        }
    }

    return 0;
}

void Memory::allocateBufferMemory(Memory::Buffer &buffer, const Memory::BufferInputChunk &input) {
    vk::MemoryRequirements memoryRequirements = input.logicalDevice.getBufferMemoryRequirements(buffer.buffer);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryTypeIndex(
            input.physicalDevice, memoryRequirements.memoryTypeBits,
            input.memoryProperties
    );

    buffer.bufferMemory = input.logicalDevice.allocateMemory(allocInfo);
    input.logicalDevice.bindBufferMemory(buffer.buffer, buffer.bufferMemory, 0);
}

Memory::Buffer Memory::createBuffer(Memory::BufferInputChunk input) {
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.flags = vk::BufferCreateFlags();
    bufferInfo.size = input.size;
    bufferInfo.usage = input.usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    Buffer buffer;
    buffer.buffer = input.logicalDevice.createBuffer(bufferInfo);

    allocateBufferMemory(buffer, input);
    return buffer;
}

void Memory::copyBuffer(Memory::Buffer &srcBuffer, Memory::Buffer &dstBuffer, vk::DeviceSize size, vk::Queue queue,
                        vk::CommandBuffer commandBuffer) {
    commandBuffer.reset();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    commandBuffer.begin(beginInfo);

    /*
    * // Provided by VK_VERSION_1_0
    typedef struct VkBufferCopy {
        VkDeviceSize    srcOffset;
        VkDeviceSize    dstOffset;
        VkDeviceSize    size;
    } VkBufferCopy;
    */
    vk::BufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);

    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    assert(queue.submit(1, &submitInfo, nullptr) == vk::Result::eSuccess);
    queue.waitIdle();
}

