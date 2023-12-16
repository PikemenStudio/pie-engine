//
// Created by Full-Hat on 02/12/2023.
//

#include "Gpu.hpp"

#include "Utils/UtilsMacros.hpp"
#include "Utils/VkLog.hpp"

#include <iomanip>


std::vector<vk::PhysicalDevice> Gpu::GetAllAvailable() {
    auto devices = static_cast<vk::Instance&>(*m_instance).enumeratePhysicalDevices();

    const auto end = std::remove_if(devices.begin(), devices.end(), [&](const vk::PhysicalDevice &el) {
        return Gpu::NoExceptCheckExtensions(el);
    });

    auto size = std::distance(devices.begin(), end);
    devices.resize(size);
    return devices;
}

Gpu::operator std::string() const {
    std::stringstream str;
    const auto& properties = m_device.getProperties();
    str << "\tapi version " << properties.apiVersion << "\n";
    str << "\tdriver version " << properties.driverVersion << "\n";
    str << "\tvendor id " << properties.vendorID << "\n";
    str << "\tdevice id " << properties.deviceID << "\n";
    str << "\tdevice type " << to_string(properties.deviceType) << "\n";
    str << "\tdevice name " << properties.deviceName << "\n";
    str << "\tpipeline cache uuid ";
    const auto uuid = properties.pipelineCacheUUID;
    for (int i = 0; i < VK_UUID_SIZE; ++i) {
        str << std::hex << std::setw(2) << std::setfill('*') << (int)uuid[i] << " ";
    }
    str << "\n\tLimits and sparse properties have too many fields" << "\n";

    return str.str();
}

void Gpu::CheckExtensions() const {
    Gpu::CheckExtensions(this->m_device);
}

void Gpu::SetCurrent(vk::PhysicalDevice _device) {
    m_device = _device;
    LOG("reset device to \n", this->ToString());

    FindAndSetQueues();

    std::vector<uint32_t> indexes;
    indexes.push_back(m_queue_indexes.graphics_queue.value());
    if (indexes[0] != m_queue_indexes.present_queue.value()) {
        indexes.push_back(m_queue_indexes.present_queue.value());
    }

    std::vector<vk::DeviceQueueCreateInfo> queues_info;
    float queue_priority { 1.0f };
    queues_info.reserve(indexes.size());
    for (auto ind : indexes) {
        queues_info.emplace_back(
            vk::DeviceQueueCreateFlags(), ind,
            1, &queue_priority
        );
    }

    vk::PhysicalDeviceFeatures device_features = vk::PhysicalDeviceFeatures();
    std::vector<const char*> enabled_layers {
        "VK_LAYER_KHRONOS_validation"
    };

    std::vector<const char*> device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    if (m_instance->IsPortabilityRequired()) {
        device_extensions.push_back("VK_KHR_portability_subset");
    }

    vk::DeviceCreateInfo device_info (
            vk::DeviceCreateFlags(),
            queues_info.size(), queues_info.data(),
            enabled_layers.size(), enabled_layers.data(),
            device_extensions.size(), device_extensions.data(),
            &device_features
    );

    try {
        m_logical_device = m_device.createDevice(device_info);
        LOG("Logical device created");
    }
    catch (vk::SystemError &err) {
        LOG("Can't create logical device", err.what());
        throw;
    }

    m_graphics_queue = m_logical_device.getQueue(m_queue_indexes.graphics_queue.value(), 0);
    m_present_queue = m_logical_device.getQueue(m_queue_indexes.present_queue.value(), 0);

    InitSwapchain();
}

void Gpu::CheckExtensions(const vk::PhysicalDevice &_device) {
    if (Gpu::NoExceptCheckExtensions(_device)) {
        throw std::runtime_error("Platform doesn't support some extensions");
    }
}

bool Gpu::NoExceptCheckExtensions(const vk::PhysicalDevice &_device) {
    std::vector<const char*> extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    LOG("Device extensions to be requested:");

    for (const char* &extensionName : extensions) {
        LOG("\t\"", extensionName, "\"");
    }

    std::vector<vk::ExtensionProperties> supported_extensions = _device.enumerateDeviceExtensionProperties();

    std::vector<const char*> not_found_exts_names;

    std::ranges::for_each(extensions, [&](std::string_view ext_name) {
        auto res = std::ranges::find_if(supported_extensions, [&](const vk::ExtensionProperties &supported_ext) {
            return supported_ext.extensionName == ext_name;
        });

        if (res == supported_extensions.cend()) {
            not_found_exts_names.push_back(ext_name.data());
        }
    });

    if (!not_found_exts_names.empty()) {
        for (const auto not_found_ext_name : not_found_exts_names) {
            LOG("Can't find ", not_found_ext_name);
        }
        return false;
    }

    return true;
}

Gpu::Gpu(std::shared_ptr<peVk::Instance> _instance, Window &_window) : m_instance(_instance),
                                                      m_vk_window(m_device, m_logical_device, _window, _instance) {
}

void Gpu::FindAndSetQueues() {
    std::vector<vk::QueueFamilyProperties> queueFamilies = m_device.getQueueFamilyProperties();

    LOG("Found", queueFamilies.size(), "queue families");

    for (unsigned int i = 0; i < queueFamilies.size(); ++i) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            m_queue_indexes.graphics_queue = i;
        }

        if (m_device.getSurfaceSupportKHR(i, m_vk_window.GetSurface())) {
            m_queue_indexes.present_queue = i;
        }
    }

    if (!m_queue_indexes.FoundAll()) {
        throw std::runtime_error("Can't find required queue family" + m_queue_indexes.GetNotSettedFamily());
    }
    LOG("OK");
}

Gpu::~Gpu() {
    m_logical_device.destroySwapchainKHR(m_swapchain);
    m_logical_device.destroy();
}

void Gpu::QuerySwapChainSupport() {
    m_swap_chain_support_details.capabilities = m_device.getSurfaceCapabilitiesKHR(m_vk_window.GetSurface());

    LOG("Swapchain can support the following surface capabilities:");

    LOG("\tminimum image count: ", m_swap_chain_support_details.capabilities.minImageCount);
    LOG("\tmaximum image count: ", m_swap_chain_support_details.capabilities.maxImageCount);

    LOG("\tcurrent extent: ");
    /*typedef struct VkExtent2D {
        uint32_t    width;
        uint32_t    height;
    } VkExtent2D;
    */
    LOG("\t\twidth: ", m_swap_chain_support_details.capabilities.currentExtent.width);
    LOG("\t\theight: ", m_swap_chain_support_details.capabilities.currentExtent.height);

    LOG("\tminimum detailsed extent: ");
    LOG("\t\twidth: ", m_swap_chain_support_details.capabilities.minImageExtent.width);
    LOG("\t\theight: ", m_swap_chain_support_details.capabilities.minImageExtent.height);

    LOG("\tmaximum detailsed extent: ");
    LOG("\t\twidth: ", m_swap_chain_support_details.capabilities.maxImageExtent.width);
    LOG("\t\theight: ", m_swap_chain_support_details.capabilities.maxImageExtent.height);

    LOG("\tmaximum image array layers: ", m_swap_chain_support_details.capabilities.maxImageArrayLayers);


    LOG("\tsupported transforms:");
    std::vector<std::string> stringList = log_transform_bits(m_swap_chain_support_details.capabilities.supportedTransforms);
    for (const std::string& line : stringList) {
        LOG("\t\t", line);
    }

    LOG("\tcurrent transform:");
    stringList = log_transform_bits(m_swap_chain_support_details.capabilities.currentTransform);
    for (const std::string& line : stringList) {
        LOG("\t\t", line);
    }

    LOG("\tsupported alpha operations:");
    stringList = log_alpha_composite_bits(m_swap_chain_support_details.capabilities.supportedCompositeAlpha);
    for (const std::string& line : stringList) {
        LOG("\t\t", line);
    }

    LOG("\tsupported image usage:");
    stringList = log_image_usage_bits(m_swap_chain_support_details.capabilities.supportedUsageFlags);
    for (const std::string& line : stringList) {
        LOG("\t\t", line);
    }

    m_swap_chain_support_details.formats = m_device.getSurfaceFormatsKHR(m_vk_window.GetSurface());
    for (vk::SurfaceFormatKHR supportedFormat : m_swap_chain_support_details.formats) {
        /*
        * typedef struct VkSurfaceFormatKHR {
            VkFormat           format;
            VkColorSpaceKHR    colorSpace;
        } VkSurfaceFormatKHR;
        */

        LOG("supported pixel format: ", vk::to_string(supportedFormat.format));
        LOG("supported color space: ", vk::to_string(supportedFormat.colorSpace));
    }

    m_swap_chain_support_details.presentModes = m_device.getSurfacePresentModesKHR(m_vk_window.GetSurface());
    for (vk::PresentModeKHR presentMode : m_swap_chain_support_details.presentModes) {
        LOG("\t", log_present_mode(presentMode));
    }
}

vk::SurfaceFormatKHR Gpu::ChooseSurfaceFormat() {
    for (vk::SurfaceFormatKHR format : m_swap_chain_support_details.formats) {
        if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }

    return m_swap_chain_support_details.formats[0];
}

vk::PresentModeKHR Gpu::ChoosePresentMode() {
    for (vk::PresentModeKHR presentMode : m_swap_chain_support_details.presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            return presentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Gpu::ChooseExtent() {
    if (m_swap_chain_support_details.capabilities.currentExtent.width != UINT32_MAX) {
        return  m_swap_chain_support_details.capabilities.currentExtent;
    }
    vk::Extent2D extent = {
            (uint32_t)m_vk_window.GetWindow().GetSize().x,
            (uint32_t)m_vk_window.GetWindow().GetSize().y
    };

    extent.width = std::min(
            m_swap_chain_support_details.capabilities.maxImageExtent.width,
            std::max(m_swap_chain_support_details.capabilities.minImageExtent.width, extent.width)
    );

    extent.height = std::min(
            m_swap_chain_support_details.capabilities.maxImageExtent.height,
            std::max(m_swap_chain_support_details.capabilities.minImageExtent.height, extent.height)
    );

    return extent;
}

void Gpu::InitSwapchain() {
    QuerySwapChainSupport();

    auto format = ChooseSurfaceFormat();
    auto mode = ChoosePresentMode();
    auto extent = ChooseExtent();

    uint32_t imageCount = std::min(
            m_swap_chain_support_details.capabilities.maxImageCount,
            m_swap_chain_support_details.capabilities.minImageCount + 1
    );

    vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
            vk::SwapchainCreateFlagsKHR(), m_vk_window.GetSurface(), imageCount, format.format, format.colorSpace,
            extent, 1, vk::ImageUsageFlagBits::eColorAttachment
    );

    assert(m_queue_indexes.FoundAll());

    uint32_t queueFamilyIndices[] = { m_queue_indexes.graphics_queue.value(), m_queue_indexes.present_queue.value() };

    if (m_queue_indexes.graphics_queue != m_queue_indexes.graphics_queue) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = m_swap_chain_support_details.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = mode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

    try {
        m_swapchain = m_logical_device.createSwapchainKHR(createInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error(std::string("failed to create swap chain!") + err.what());
    }

    m_swapchain_images = m_logical_device.getSwapchainImagesKHR(m_swapchain);
    m_swapchain_format = format.format;

    m_swapchain_extent = extent;
}
