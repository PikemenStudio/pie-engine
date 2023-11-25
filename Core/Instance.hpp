//
// Created by nikit on 11/24/2023.
//

#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace peVk {

class Instance {
public:
    Instance(std::string_view _application_name);
protected:
    vk::Instance m_vk_instance { nullptr };

    struct Version {
        uint32_t version;

        Version(uint32_t Variant, uint32_t Major, uint32_t Minor, uint32_t Patch) {
            version = VK_MAKE_API_VERSION(Variant, Major, Minor, Patch);
        }

        Version() {
            vkEnumerateInstanceVersion(&version);
        }

        operator uint32_t () {
            return version;
        }
    } m_version;
};

} // peVk
