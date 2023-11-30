//
// Created by Full-Hat on 30/11/2023.
//

#include "DebugMessanger.hpp"


VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    LOG("validation layer: ", pCallbackData->pMessage);

    return VK_FALSE;
}

vk::DebugUtilsMessengerEXT make_debug_messenger(vk::Instance *instance, vk::DispatchLoaderDynamic &dldi) {
    vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            debugCallback,
            nullptr
    );

    return instance->createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
}
