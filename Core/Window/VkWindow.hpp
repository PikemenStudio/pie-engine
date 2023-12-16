//
// Created by Full-Hat on 03/12/2023.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include "Core/Window/Window.hpp"
#include "Core/Instance.hpp"

class VkWindow {
public:
    VkWindow(vk::PhysicalDevice &_device, vk::Device &_logical_device, Window &_window, std::shared_ptr<peVk::Instance> _instance);
    ~VkWindow();

    vk::SurfaceKHR& GetSurface() { return m_surface; }
    Window& GetWindow() { return m_window; }
    
protected:
    vk::SurfaceKHR m_surface;

    vk::PhysicalDevice &m_device;
    vk::Device &m_logical_device;

    Window &m_window;
    std::shared_ptr<peVk::Instance> m_instance;
};
