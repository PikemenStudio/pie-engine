//
// Created by nikit on 11/24/2023.
//

#include "Instance.hpp"

#include <iostream>

namespace peVk {

Instance::Instance(std::string_view _application_name) {
    m_version = Version();

    vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
            dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    uint32_t version;
    vk::enumerateInstanceVersion(&version);
    std::cout << VK_API_VERSION_PATCH(version);

//    vk::ApplicationInfo app_inf {
//        _application_name.data(),
//        m_version,
//        "",
//        m_version,
//        m_version
//    };
}

} // peVk