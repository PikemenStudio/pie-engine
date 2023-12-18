//
// Created by Full-Hat on 17/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include <string>

#include "Shaders/Shader.hpp"

class GraphicsPipeline {
public:
    struct GraphicsPipelineInBundle;

    GraphicsPipeline(GraphicsPipelineInBundle& specification);
    ~GraphicsPipeline();

    struct GraphicsPipelineInBundle {
        GraphicsPipelineInBundle(vk::Device &_device) : device(_device) {
        }
        vk::Device &device;
        std::string vertexFilepath;
        std::string fragmentFilepath;
        vk::Extent2D swapchainExtent;
        vk::Format swapchainImageFormat;
    };

    [[nodiscard]]
    vk::PipelineLayout GetPipelineLayout() const {
        return m_bundle.layout;
    }

    [[nodiscard]]
    vk::RenderPass GetRenderPass() const {
        return m_bundle.renderpass;
    }

    [[nodiscard]]
    vk::Pipeline& GetPipeline() {
        return m_bundle.pipeline;
    }

protected:
    [[nodiscard]]
    vk::PipelineLayout init_pipeline_layout(const vk::Device &device);
    [[nodiscard]]
    vk::RenderPass init_renderpass(const vk::Device &device, vk::Format swapchainImageFormat);

    void init_graphics_pipeline(const GraphicsPipelineInBundle& specification);

protected:

    struct ShadersPack {
        using shader_ptr = std::unique_ptr<Shader>;
        shader_ptr vertex;
        shader_ptr fragment;
    } m_shaders;

    struct GraphicsPipelineOutBundle {
        vk::PipelineLayout layout;
        vk::RenderPass renderpass;
        vk::Pipeline pipeline;
    } m_bundle;

    vk::Device &m_device;
};
