//
// Created by nikit on 11/24/2023.
//

#include "Instance.hpp"

#include <iostream>

namespace peVk {

Instance::Instance(std::string_view _application_name) {
    m_version = Version();

    vk::ApplicationInfo app_inf {
        _application_name.data(),
        m_version,
        "pieEngine",
        m_version,
        m_version
    };
}

} // peVk