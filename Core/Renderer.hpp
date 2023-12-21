//
// Created by Full-Hat on 20/12/2023.
//

#pragma once

#include "Core/Gpu.hpp"
#include "Shaders/Pipeline.hpp"
#include "Project/MeshesManager.hpp"

#include "vulkan/vulkan.hpp"

#include <memory>

class Renderer {
public:
    struct RendererCreateInfo {
        peVk::Instance m_instance;
        std::shared_ptr<GlfwWindow> m_window;
    };

    explicit Renderer(RendererCreateInfo _info) :
            m_instance(_info.m_instance),
            m_window(_info.m_window) {
    }
    ~Renderer();

public:
    void Render();

    void SetSceneReference(Scene *_scene);

public:
    // Inits
    void InitGpu();
    void InitPipeline();
    void InitBuffers();

protected:
    // Fields
    std::unique_ptr<Gpu> m_gpu;
    std::unique_ptr<GraphicsPipeline> m_pipeline;

    // References
    peVk::Instance m_instance;
    std::shared_ptr<GlfwWindow> m_window;
    Scene *m_scene;
};
