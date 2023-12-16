//
// Created by Full-Hat on 02/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "Core/Instance.hpp"
#include "Core/Window/GlfwWindow.hpp"

#include <optional>

class Gpu {
public:
    explicit Gpu(std::shared_ptr<peVk::Instance> _instance, GlfwWindow &_window);
    ~Gpu();

    std::vector<vk::PhysicalDevice> GetAllAvailable();
    void SetCurrent(vk::PhysicalDevice _device);

    explicit operator std::string() const;
    [[nodiscard]] std::string ToString() const { return (std::string)*this; }

    void CheckExtensions() const;
    static void CheckExtensions(const vk::PhysicalDevice &_device);
    static bool NoExceptCheckExtensions(const vk::PhysicalDevice &_device);

    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

protected:
    void FindAndSetQueues();

    void QuerySwapChainSupport();

    vk::SurfaceFormatKHR ChooseSurfaceFormat();
    vk::PresentModeKHR ChoosePresentMode();
    vk::Extent2D ChooseExtent();

    void InitSwapchain();

    void InitSurface();

protected:
    vk::PhysicalDevice m_device { nullptr };
    vk::Device m_logical_device { nullptr };
    vk::Queue m_graphics_queue { nullptr };
    vk::Queue m_present_queue { nullptr };

    vk::SwapchainKHR m_swapchain{ nullptr };
    std::vector<vk::Image> m_swapchain_images{ nullptr };
    vk::Format m_swapchain_format;
    vk::Extent2D m_swapchain_extent;

    vk::SurfaceKHR m_surface;

    SwapChainSupportDetails m_swap_chain_support_details;

    struct QueueIndexes {
        std::optional<uint32_t> graphics_queue;
        std::optional<uint32_t> present_queue;

        bool FoundAll() const {
            return graphics_queue.has_value() && present_queue.has_value();
        }

        std::string GetNotSettedFamily() {
            std::stringstream str;
            if (!graphics_queue.has_value()) {
                str << "Graphics" << "\n";
            }
            if (!present_queue.has_value()) {
                str << "Present" << "\n";
            }
            return str.str();
        }
    } m_queue_indexes;

    std::shared_ptr<peVk::Instance> m_instance;

    GlfwWindow &m_window;
};
