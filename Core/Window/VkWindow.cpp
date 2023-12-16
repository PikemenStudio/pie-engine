//
// Created by Full-Hat on 03/12/2023.
//

#include "VkWindow.hpp"
#include "Utils/UtilsMacros.hpp"

#include <GLFW/glfw3.h>

#include <utility>

VkWindow::VkWindow(vk::PhysicalDevice &_device, vk::Device &_logical_device, Window &_window, std::shared_ptr<peVk::Instance> _instance) :
        m_device(_device), m_logical_device(_logical_device), m_window(_window), m_instance(std::move(_instance)) {
    VkSurfaceKHR c_style_surface;
    if (glfwCreateWindowSurface(*m_instance->ToVkInstancePtr(), &m_window.ToWindow(), nullptr, &c_style_surface) != VK_SUCCESS) {
        throw std::runtime_error("Can't create surface");
    }
    m_surface = c_style_surface;

    LOG("OK", "surface created");
}

VkWindow::~VkWindow() {
    m_instance->ToVkInstancePtr()->destroySurfaceKHR(m_surface);
}
