//
// Created by Full-Hat on 17/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

class Shader {
public:
    Shader(const std::string &_file_name, const vk::Device &device);

    vk::ShaderModule GetModule() const {
        return m_module;
    }

    static void compile_shader(const std::string _filepath, const std::string& _outputFolder);

protected:
    vk::ShaderModule m_module;
};
