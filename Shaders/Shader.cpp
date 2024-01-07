//
// Created by Full-Hat on 17/12/2023.
//

#include "Shader.hpp"

#include "Utils/File.hpp"
#include "Utils/Process.hpp"

#include <filesystem>

Shader::Shader(const std::string &_file_name, const vk::Device &device) {
    std::vector<char> sourceCode = File::read(_file_name);
    vk::ShaderModuleCreateInfo moduleInfo = {};
    moduleInfo.flags = vk::ShaderModuleCreateFlags();
    moduleInfo.codeSize = sourceCode.size();
    moduleInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data());

    try {
        m_module = device.createShaderModule(moduleInfo);
    }
    catch (vk::SystemError err) {
        LOG("Failed to create shader module for \"", _file_name, "\"");
    }
    catch (...) {
        // File reader error
        // Or something system :D
        throw;
    }
}

void Shader::compile_shader(const std::string _filepath, const std::string& _outputFolder) {
    constexpr std::string_view vulkanSdkPath = VULKAN_SDK_PATH;

    // Construct the full path to the output file
    std::filesystem::path outputPath = std::filesystem::path(_outputFolder) / std::filesystem::path(_filepath).filename();
    outputPath.replace_extension(".spv");

    // Construct the full command to compile the shader
    std::string command;

#ifdef _WIN32
    command = std::string(vulkanSdkPath) + "\\bin\\glslc.exe " + _filepath + " -o " + outputPath.string();
#else
    command = std::string(vulkanSdkPath) + "/bin/glslc " + _filepath + " -o " + outputPath.string();
#endif

    try {
        execute(command);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}
