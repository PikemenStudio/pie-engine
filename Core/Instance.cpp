//
// Created by nikit on 11/24/2023.
//

#include "Instance.hpp"

#include "Utils/UtilsMacros.hpp"

#include "GLFW/glfw3.h"

#include <iostream>

namespace peVk {

Instance::Instance(std::string_view _application_name) {
    m_version = Version();

    LOG("System support Vulkan",
        "\n\tVariant ", m_version.get_variant(),
        "\n\tMajor", m_version.get_major(),
        "\n\tMinor", m_version.get_minor(),
        "\n\tPatch", m_version.get_patch());

    vk::ApplicationInfo app_inf {
        _application_name.data(),
        m_version,
        "pie engine",
        m_version,
        m_version
    };

    // Get list of extensions
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    if (m_version >= VK_MAKE_API_VERSION(0, 1, 3, 216)) {
        extensions.push_back("VK_KHR_portability_enumeration");
    }
    extensions.push_back("VK_EXT_debug_utils");

    LOG("Extensions to be requested:");

    for (const char* extensionName : extensions) {
        LOG("\t\"", extensionName, "\"");
    }

    std::vector<const char*> layers;
    layers.push_back("VK_LAYER_KHRONOS_validation");

    // Load extensions
    vk::InstanceCreateInfo create_inf = vk::InstanceCreateInfo(
            vk::InstanceCreateFlags(),
            &app_inf,
            static_cast<uint32_t>(layers.size()), layers.data(), // enabled layers
            static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
    );

    if (m_version >= VK_MAKE_API_VERSION(0, 1, 3, 216)) {
        create_inf.flags = vk::InstanceCreateFlags(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR);
    }

    try {
        CheckForExtensionSupport(extensions);
        CheckForLayersSupport(layers);
        m_vk_instance = vk::createInstance(create_inf);
    }
    catch(vk::SystemError &err) {
        LOG("ERROR", err.what());
        throw;
    }
    catch(std::runtime_error &err) {
        LOG("ERROR", err.what());
        throw;
    }

    // Check for support
}

    void Instance::CheckForExtensionSupport(const std::vector<const char *> &_ext_name) {
        std::vector<vk::ExtensionProperties> supported_extensions = vk::enumerateInstanceExtensionProperties();

        std::vector<const char*> not_found_exts_names;

        std::ranges::for_each(_ext_name, [&](std::string_view ext_name) {
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
            throw std::runtime_error("Platform doesn't support some extensions");
        }
    }

    void Instance::CheckForLayersSupport(std::vector<const char *> &_required_layers) {
        std::vector<vk::LayerProperties> supported_layers = vk::enumerateInstanceLayerProperties();

        std::vector<const char*> not_found_layers_names;

        std::ranges::for_each(_required_layers, [&](std::string_view layer_name) {
            auto res = std::ranges::find_if(supported_layers, [&](const vk::LayerProperties &supported_layer) {
                return supported_layer.layerName == layer_name;
            });

            if (res == supported_layers.cend()) {
                not_found_layers_names.push_back(layer_name.data());
            }
        });

        if (!not_found_layers_names.empty()) {
            for (const auto not_found_layer_name : not_found_layers_names) {
                LOG("Can't find ", not_found_layer_name);
            }
            throw std::runtime_error("Platform doesn't support some layers");
        }
    }

} // peVk