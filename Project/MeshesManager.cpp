//
// Created by Full-Hat on 19/12/2023.
//

#include "MeshesManager.hpp"

MeshesManager::MeshesManager() {
    offset = 0;
}

void MeshesManager::Consume(Mesh::Types type, std::vector<float> vertexData) {
    for (float attribute : vertexData) {
        lump.push_back(attribute);
    }
    int vertexCount = static_cast<int>(vertexData.size() / 5);

    offsets.insert(std::make_pair(type, offset));
    sizes.insert(std::make_pair(type, vertexCount));

    offset += vertexCount;
}

void MeshesManager::Finalize(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice) {
    this->logicalDevice = logicalDevice;

    Memory::BufferInputChunk inputChunk;
    inputChunk.logicalDevice = logicalDevice;
    inputChunk.physicalDevice = physicalDevice;
    inputChunk.size = sizeof(float) * lump.size();
    inputChunk.usage = vk::BufferUsageFlagBits::eVertexBuffer;

    vertexBuffer = Memory::createBuffer(inputChunk);

    void* memoryLocation = logicalDevice.mapMemory(vertexBuffer.bufferMemory, 0, inputChunk.size);
    memcpy(memoryLocation, lump.data(), inputChunk.size);
    logicalDevice.unmapMemory(vertexBuffer.bufferMemory);
}

MeshesManager::~MeshesManager() {
    logicalDevice.destroyBuffer(vertexBuffer.buffer);
    logicalDevice.freeMemory(vertexBuffer.bufferMemory);
}


