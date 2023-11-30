//
// Created by Full-Hat on 30/11/2023.
//

#pragma once

#include <vulkan/vulkan.hpp>

#include "Utils/UtilsMacros.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
);

vk::DebugUtilsMessengerEXT make_debug_messenger(vk::Instance *instance, vk::DispatchLoaderDynamic& dldi);
