//
// Created by Full-Hat on 02/12/2023.
//

#include "gpu.hpp"

#include "Utils/UtilsMacros.hpp"

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

Gpu::Gpu(std::shared_ptr<peVk::Instance> _instance) : m_instance(_instance) {

}
