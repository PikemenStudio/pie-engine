//
// Created by nikit on 11/24/2023.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace peVk {

class Instance {
public:
    explicit Instance(std::string_view _application_name);
protected:
    vk::Instance m_vk_instance { nullptr };

    struct Version {
        uint32_t version = 0;

        Version(uint32_t Variant, uint32_t Major, uint32_t Minor, uint32_t Patch) {
            version = VK_MAKE_API_VERSION(Variant, Major, Minor, Patch);
        }

        Version() {
            vkEnumerateInstanceVersion(&version);
        }

        operator uint32_t () const {
            return version;
        }
    } m_version;
};

} // peVk
