//
// Created by Full-Hat on 02/12/2023.
//

#include "Gpu.hpp"

#include "Utils/UtilsMacros.hpp"
#include "Utils/VkLog.hpp"
#include "Utils/Sync.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "Shaders/Shaders/empty.hpp"

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

Gpu::Gpu(std::shared_ptr<peVk::Instance> _instance, GlfwWindow &_window) : m_instance(_instance),
                                                                           m_window(_window) {
    InitSurface();
}

void Gpu::FindAndSetQueues() {
    std::vector<vk::QueueFamilyProperties> queueFamilies = m_device.getQueueFamilyProperties();

    LOG("Found", queueFamilies.size(), "queue families");

    for (unsigned int i = 0; i < queueFamilies.size(); ++i) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            m_queue_indexes.graphics_queue = i;
        }

        if (m_device.getSurfaceSupportKHR(i, m_surface)) {
            m_queue_indexes.present_queue = i;
        }
    }

    if (!m_queue_indexes.FoundAll()) {
        throw std::runtime_error("Can't find required queue family" + m_queue_indexes.GetNotSettedFamily());
    }
    LOG("OK");
}

Gpu::~Gpu() {
    for (auto frame : m_swap_chain.m_frames) {
        m_logical_device.destroyImageView(frame.image_view);
        m_logical_device.destroyFramebuffer(frame.frame_buffer);

        m_logical_device.destroyFence(frame.inFlightFence);
        m_logical_device.destroySemaphore(frame.imageAvailable);
        m_logical_device.destroySemaphore(frame.renderFinished);
    }

    m_logical_device.destroySwapchainKHR(m_swap_chain.m_swap_chain);
    m_logical_device.destroy();

    m_instance->ToVkInstancePtr()->destroySurfaceKHR(m_surface);
}

void Gpu::QuerySwapChainSupport() {
    m_swap_chain_support_details.capabilities = m_device.getSurfaceCapabilitiesKHR(m_surface);

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

    m_swap_chain_support_details.formats = m_device.getSurfaceFormatsKHR(m_surface);
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

    m_swap_chain_support_details.presentModes = m_device.getSurfacePresentModesKHR(m_surface);
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
//    // !!! Force mailbox
//    return vk::PresentModeKHR::eMailbox;

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
            (uint32_t)m_window.GetSize().x,
            (uint32_t)m_window.GetSize().y
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
            vk::SwapchainCreateFlagsKHR(), m_surface, imageCount, format.format, format.colorSpace,
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
        m_swap_chain.m_swap_chain = m_logical_device.createSwapchainKHR(createInfo);
    }
    catch (vk::SystemError &err) {
        throw std::runtime_error(std::string("failed to create swap chain!") + err.what());
    }

    auto images = m_logical_device.getSwapchainImagesKHR(m_swap_chain.m_swap_chain);
    m_swap_chain.m_frames.reserve(images.size());
    for (auto image : images) {
        SwapChainFrame frame{};

        vk::ImageViewCreateInfo imageCreateInfo = {};
        imageCreateInfo.image = image;
        imageCreateInfo.viewType = vk::ImageViewType::e2D;
        imageCreateInfo.format = format.format;
        imageCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
        imageCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
        imageCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
        imageCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
        imageCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageCreateInfo.subresourceRange.baseMipLevel = 0;
        imageCreateInfo.subresourceRange.levelCount = 1;
        imageCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageCreateInfo.subresourceRange.layerCount = 1;

        frame.m_image = image;
        frame.image_view = m_logical_device.createImageView(imageCreateInfo);

        m_swap_chain.m_frames.push_back(frame);
    }
    m_swap_chain.m_format = format.format;

    m_swap_chain.m_extent = extent;

    MaxFramesInFlight = m_swap_chain.m_frames.size();
    FrameNumber = 0;
}

void Gpu::InitSurface() {
    VkSurfaceKHR c_style_surface;
    if (glfwCreateWindowSurface(*m_instance->ToVkInstancePtr(), &m_window.ToWindow(), nullptr, &c_style_surface) != VK_SUCCESS) {
        throw std::runtime_error("Can't create surface");
    }
    m_surface = c_style_surface;

    LOG("OK", "surface created");
}

void Gpu::make_command_pool() {

    QueueIndexes queueFamilyIndices = m_queue_indexes;

    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_queue.value();

    try {
        m_command_pool = m_logical_device.createCommandPool(poolInfo);
    }
    catch (vk::SystemError &err) {
        LOG("Failed to create Command Pool");
        throw;
    }
}

void Gpu::make_framebuffers(Gpu::framebufferInput _input_chunk) {
    for (int i = 0; i < m_swap_chain.m_frames.size(); ++i) {

        std::vector<vk::ImageView> attachments = {
                m_swap_chain.m_frames[i].image_view
        };

        vk::FramebufferCreateInfo framebufferInfo;
        framebufferInfo.flags = vk::FramebufferCreateFlags();
        framebufferInfo.renderPass = _input_chunk.renderpass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _input_chunk.swapchainExtent.width;
        framebufferInfo.height = _input_chunk.swapchainExtent.height;
        framebufferInfo.layers = 1;

        try {
            m_swap_chain.m_frames[i].frame_buffer = _input_chunk.device.createFramebuffer(framebufferInfo);

            LOG("Created framebuffer for frame ", i);
        }
        catch (vk::SystemError &err) {
            LOG("Failed to create framebuffer for frame ", i);
            throw;
        }

    }
}

void Gpu::make_command_buffers() {

    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = m_command_pool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    //Make a command buffer for each frame
    for (int i = 0; i < m_swap_chain.m_frames.size(); ++i) {
        try {
            m_swap_chain.m_frames[i].command_buffer = m_logical_device.allocateCommandBuffers(allocInfo)[0];

            LOG("Allocated command buffer for frame ", i);
        }
        catch (vk::SystemError &err) {
            LOG("Failed to allocate command buffer for frame ", i);
        }
    }


    //Make a "main" command buffer for the engine
    try {
        m_command_buffer = m_logical_device.allocateCommandBuffers(allocInfo)[0];

        LOG("Allocated main command buffer ");
    }
    catch (vk::SystemError &err) {
        LOG("Failed to allocate main command buffer");
        throw;
    }
}

void Gpu::make_syncs_objs() {
    for (auto &frame : m_swap_chain.m_frames) {
        frame.inFlightFence = Sync::make_fence(m_logical_device);
        frame.imageAvailable = Sync::make_semaphore(m_logical_device);
        frame.renderFinished = Sync::make_semaphore(m_logical_device);
    }
}

void Gpu::Render(GraphicsPipeline &pipeline, Scene _scene) {
    assert(m_logical_device.waitForFences(1, &m_swap_chain.m_frames[FrameNumber].inFlightFence, VK_TRUE, UINT64_MAX) == vk::Result::eSuccess);
    assert(m_logical_device.resetFences(1, &m_swap_chain.m_frames[FrameNumber].inFlightFence) == vk::Result::eSuccess);

    //acquireNextImageKHR(vk::SwapChainKHR, timeout, semaphore_to_signal, fence)
    uint32_t imageIndex{
        m_logical_device.acquireNextImageKHR(m_swap_chain.m_swap_chain,
                                             UINT64_MAX,
                                             m_swap_chain.m_frames[FrameNumber].imageAvailable,
                                             nullptr).value
    };

    vk::CommandBuffer commandBuffer = m_swap_chain.m_frames[FrameNumber].command_buffer;

    commandBuffer.reset();

    RecordDrawCommand(commandBuffer, imageIndex, pipeline, _scene);

    vk::SubmitInfo submitInfo = {};

    vk::Semaphore waitSemaphores[] = { m_swap_chain.m_frames[FrameNumber].imageAvailable };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk::Semaphore signalSemaphores[] = { m_swap_chain.m_frames[FrameNumber].renderFinished };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    try {
        m_graphics_queue.submit(submitInfo, m_swap_chain.m_frames[FrameNumber].inFlightFence);
    }
    catch (vk::SystemError &err) {
        LOG("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = { m_swap_chain.m_swap_chain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    assert(m_present_queue.presentKHR(presentInfo) == vk::Result::eSuccess);

    FrameNumber = (FrameNumber + 1) % MaxFramesInFlight;
}

#include <thread>

void Gpu::RecordDrawCommand(vk::CommandBuffer command_buffer, uint32_t image_index, GraphicsPipeline &pipeline, Scene _scene) {
    vk::CommandBufferBeginInfo beginInfo = {};

    try {
        command_buffer.begin(beginInfo);
    }
    catch (vk::SystemError &err) {
        LOG("Failed to begin recording command buffer!");
    }

    vk::RenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.renderPass = pipeline.GetRenderPass();
    renderPassInfo.framebuffer = m_swap_chain.m_frames[image_index].frame_buffer;
    renderPassInfo.renderArea.offset.x = 0;
    renderPassInfo.renderArea.offset.y = 0;
    renderPassInfo.renderArea.extent = m_swap_chain.m_extent;

    vk::ClearValue clearColor = { std::array<float, 4>{1.0f, 0.5f, 0.25f, 1.0f} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    command_buffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());

    for (auto position : _scene.m_positions) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        ObjectData objectData;
        objectData.model = model;
        command_buffer.pushConstants(pipeline.GetPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(objectData), &objectData);
        command_buffer.draw(3, 1, 0, 0);
    }

    command_buffer.endRenderPass();

    try {
        command_buffer.end();
    }
    catch (vk::SystemError &err) {
        LOG("failed to record command buffer!");
    }
}

void Gpu::DestroyCommandPool() {
    m_logical_device.waitIdle();
    m_logical_device.destroyCommandPool(m_command_pool);
}
