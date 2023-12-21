//
// Created by Full-Hat on 19/12/2023.
//

#include "MeshesManager.hpp"

#include "Utils/UtilsMacros.hpp"

MeshesManager::MeshesManager() {
    offset = 0;
}

void MeshesManager::Consume(std::string type, std::vector<float> vertexData) {
    for (float attribute : vertexData) {
        lump.push_back(attribute);
    }
    int vertexCount = static_cast<int>(vertexData.size() / 5);

    DataUnit data_unit;
    data_unit.size = vertexCount;
    data_unit.offset = offset;

    data.insert({ type, data_unit });

    offset += vertexCount;
}

void MeshesManager::Finalize(FinalizationChunk finalizationChunk) {
    this->logicalDevice = finalizationChunk.logicalDevice;

    Memory::BufferInputChunk inputChunk;
    inputChunk.logicalDevice = finalizationChunk.logicalDevice;
    inputChunk.physicalDevice = finalizationChunk.physicalDevice;
    inputChunk.size = sizeof(float) * lump.size();
    inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
    inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    Memory::Buffer stagingBuffer = Memory::createBuffer(inputChunk);

    void* memoryLocation = logicalDevice.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
    memcpy(memoryLocation, lump.data(), inputChunk.size);
    logicalDevice.unmapMemory(stagingBuffer.bufferMemory);

    inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    vertexBuffer = Memory::createBuffer(inputChunk);

    Memory::copyBuffer(stagingBuffer, vertexBuffer, inputChunk.size, finalizationChunk.queue, finalizationChunk.commandBuffer);

    logicalDevice.destroyBuffer(stagingBuffer.buffer);
    logicalDevice.freeMemory(stagingBuffer.bufferMemory);
}

MeshesManager::~MeshesManager() {
    LOG("Destroy mesh manager");
    logicalDevice.destroyBuffer(vertexBuffer.buffer);
    logicalDevice.freeMemory(vertexBuffer.bufferMemory);
}


