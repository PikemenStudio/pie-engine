//
// Created by Full-Hat on 02/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "Core/Instance.hpp"

class Gpu {
public:
    explicit Gpu(std::shared_ptr<peVk::Instance> _instance);

    std::vector<vk::PhysicalDevice> GetAllAvailable();
    void SetCurrent(vk::PhysicalDevice _device);

    explicit operator std::string() const;
    [[nodiscard]] std::string ToString() const { return (std::string)*this; }

    void CheckExtensions() const;
    static void CheckExtensions(const vk::PhysicalDevice &_device);
    static bool NoExceptCheckExtensions(const vk::PhysicalDevice &_device);

protected:
    vk::PhysicalDevice m_device { nullptr };

    std::shared_ptr<peVk::Instance> m_instance;
};
