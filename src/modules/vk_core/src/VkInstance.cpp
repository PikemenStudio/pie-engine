//
// Created by FullHat on 30/03/2024.
//

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VkInstance.h"

namespace vk_core {
VkInstance::VkInstance(VkInstance::VkInstanceProps Props) {
  vkEnumerateInstanceVersion(&Version);

  vk::ApplicationInfo AppInf {
      .pApplicationName = Props.AppName.c_str(),
      .applicationVersion = Props.AppVersion.toVkVersion(),
      .pEngineName = Props.EngineName.c_str(),
      .engineVersion = Props.EngineVersion.toVkVersion(),
      .apiVersion = Version
  };

//  std::vector<const char*> extensions(Props., GlfwExtensions + GlfwExtensionCount);
//  if (m_version >= VK_MAKE_API_VERSION(0, 1, 3, 216)) {
//    extensions.push_back("VK_KHR_portability_enumeration");
//  }
//  extensions.push_back("VK_EXT_debug_utils");
//
//  LOG("Extensions to be requested:");
//
//  for (const char* extensionName : extensions) {
//    LOG("\t\"", extensionName, "\"");
//  }
}
} // namespace vk_core