//
// Created by nikit on 11/24/2023.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace peVk {

class Instance {
public:
    explicit Instance(std::string_view _application_name);

    static void CheckForExtensionSupport(const std::vector<const char *> &_ext_name);
    static void CheckForLayersSupport(std::vector<const char *> &_required_layers);

    explicit operator vk::Instance&() {
        return m_vk_instance;
    }

    vk::Instance* ToVkInstancePtr() {
        return &m_vk_instance;
    }
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

        [[nodiscard]] uint32_t get_variant() const {
            return VK_VERSION_MAJOR(version);
        }

        [[nodiscard]] uint32_t get_major() const {
            return VK_VERSION_MAJOR(version);
        }

        [[nodiscard]] uint32_t get_minor() const {
            return VK_VERSION_MINOR(version);
        }

        [[nodiscard]] uint32_t get_patch() const {
            return VK_VERSION_PATCH(version);
        }
    } m_version;
};

} // peVk
