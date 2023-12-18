//
// Created by Full-Hat on 17/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "Utils/UtilsMacros.hpp"

class Sync {
public:
    static vk::Semaphore make_semaphore(vk::Device device) {

        vk::SemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.flags = vk::SemaphoreCreateFlags();

        try {
            return device.createSemaphore(semaphoreInfo);
        }
        catch (vk::SystemError &err) {
            LOG("Failed to create semaphore");
            return nullptr;
        }
    }

    static vk::Fence make_fence(vk::Device device) {

        vk::FenceCreateInfo fenceInfo = {};
        fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

        try {
            return device.createFence(fenceInfo);
        }
        catch (vk::SystemError &err) {
            LOG("Failed to create fence");
            return nullptr;
        }
    }
};
