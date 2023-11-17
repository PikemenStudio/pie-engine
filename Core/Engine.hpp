#pragma once

#include <vulkan/vulkan.hpp>

class Engine {
 public:
  void RunMainCycle();

  protected:
	  void BuildInstance() {

  }
 protected:
  vk::Instance m_instance{nullptr};
};
