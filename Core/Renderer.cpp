//
// Created by Full-Hat on 20/12/2023.
//

#include "Renderer.hpp"

#include "Utils/UtilsMacros.hpp"

void Renderer::InitGpu() {
    // Create gpu
    m_gpu = std::make_unique<Gpu>(std::make_shared<peVk::Instance>(m_instance), m_window);

    // Choose gpu
    auto devices = m_gpu->GetAllAvailable();
    m_gpu->SetCurrent(devices[0]);
}

void Renderer::InitPipeline() {
    // Must be inited
    assert(m_gpu != nullptr);

#ifdef _WIN32
    const std::string shaderDir = std::string(PROJECT_SOURCE_DIR) + "\\Shaders\\Shaders\\";
#else
    const std::string shaderDir = std::string(PROJECT_SOURCE_DIR) + "/Shaders/Shaders/";
#endif
    
    // Make shader system
    GraphicsPipeline::GraphicsPipelineInBundle specification(m_gpu->GetLogicalDevice());
    Shader::compile_shader(shaderDir + "empty_f.frag", "./");
    Shader::compile_shader(shaderDir + "empty_v.vert", "./");
    specification.vertexFilepath = "./empty_v.spv";
    specification.fragmentFilepath = "./empty_f.spv";
    specification.swapchainExtent = m_gpu->GetSwapChainExtent();
    specification.swapchainImageFormat = m_gpu->GetSwapChainFormat();

    m_pipeline = std::make_unique<GraphicsPipeline>(specification);
}

void Renderer::InitBuffers() {
    // Must be inited
    assert(m_gpu != nullptr);
    // Must be inited
    assert(m_pipeline != nullptr);

    Gpu::framebufferInput frameBufferInput;
    frameBufferInput.device = m_gpu->GetLogicalDevice();
    frameBufferInput.renderpass = m_pipeline->GetRenderPass();
    frameBufferInput.swapchainExtent = m_gpu->GetSwapChainExtent();

    m_gpu->make_framebuffers(frameBufferInput);

    m_gpu->make_command_pool();
    m_gpu->make_main_command_buffer();
    m_gpu->make_frame_command_buffers();
}

void Renderer::Render() {
    // Render frame
    m_gpu->Render(*m_pipeline, *m_scene);
}

void Renderer::SetSceneReference(Scene *_scene) {
    m_scene = _scene;

    m_scene->DestroyMeshesSystem();

    // Finalize meshes
    MeshesManager::FinalizationChunk finalizationInfo;
    finalizationInfo.logicalDevice = m_gpu->GetLogicalDevice();
    finalizationInfo.physicalDevice = m_gpu->GetDevice();
    finalizationInfo.commandBuffer = m_gpu->GetCommandBuffer();
    finalizationInfo.queue = m_gpu->GetGraphicsQueue();

    m_scene->GetMeshes().Finalize(finalizationInfo);
}

Renderer::~Renderer() {
    LOG("Destroy renderer");
    m_gpu->DestroyCommandPool();
    m_pipeline.reset();
    m_scene->DestroyMeshesSystem();
    m_gpu->DestroySwapchain();
    m_gpu.reset();
    m_window.reset();
}
