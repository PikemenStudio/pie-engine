//
// Created by Full-Hat on 17/12/2023.
//

#include "Pipeline.hpp"

#include "Utils/UtilsMacros.hpp"
#include "Shaders/Shaders/empty.hpp"
#include "Project/Mesh.hpp"

vk::PipelineLayout GraphicsPipeline::init_pipeline_layout(const vk::Device &device) {
    vk::PipelineLayoutCreateInfo layoutInfo;
    layoutInfo.flags = vk::PipelineLayoutCreateFlags();
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pushConstantRangeCount = 1;
    vk::PushConstantRange pushConstantInfo;
    pushConstantInfo.offset = 0;
    pushConstantInfo.size = sizeof(ObjectData);
    pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;
    layoutInfo.pPushConstantRanges = &pushConstantInfo;

    try {
        return device.createPipelineLayout(layoutInfo);
    }
    catch (vk::SystemError err) {
        LOG("Failed to create pipeline layout!", "err message:", err.what());
        throw;
    }
}

vk::RenderPass GraphicsPipeline::init_renderpass(const vk::Device &device, vk::Format swapchainImageFormat) {
    //Define a general attachment, with its load/store operations
    vk::AttachmentDescription colorAttachment = {};
    colorAttachment.flags = vk::AttachmentDescriptionFlags();
    colorAttachment.format = swapchainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    //Declare that attachment to be color buffer 0 of the framebuffer
    vk::AttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    //Renderpasses are broken down into subpasses, there's always at least one.
    vk::SubpassDescription subpass = {};
    subpass.flags = vk::SubpassDescriptionFlags();
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    //Now create the renderpass
    vk::RenderPassCreateInfo renderpassInfo = {};
    renderpassInfo.flags = vk::RenderPassCreateFlags();
    renderpassInfo.attachmentCount = 1;
    renderpassInfo.pAttachments = &colorAttachment;
    renderpassInfo.subpassCount = 1;
    renderpassInfo.pSubpasses = &subpass;
    try {
        return device.createRenderPass(renderpassInfo);
    }
    catch (vk::SystemError err) {
        LOG("Failed to create renderpass!", "err message:", err.what());
        throw;
    }
}

void GraphicsPipeline::init_graphics_pipeline(const GraphicsPipeline::GraphicsPipelineInBundle &specification) {
    //The info for the graphics pipeline
    vk::GraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.flags = vk::PipelineCreateFlags();

    //Shader stages, to be populated later
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

    //Vertex Input
    auto PosColorBindingDescription = Mesh::GetPosColorBindingDescription();
    auto PosColorAttributeDescriptions = Mesh::GetPosColorAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &PosColorBindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = PosColorAttributeDescriptions.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    //Input Assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
    inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
    inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

    //Vertex Shader
    LOG("Create vertex shader module");
    m_shaders.vertex = std::make_unique<Shader>(specification.vertexFilepath, specification.device);

    vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
    vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
    vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertexShaderInfo.module = m_shaders.vertex->GetModule();
    vertexShaderInfo.pName = "main";
    shaderStages.push_back(vertexShaderInfo);

    //Viewport and Scissor
    vk::Viewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)specification.swapchainExtent.width;
    viewport.height = (float)specification.swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vk::Rect2D scissor = {};
    scissor.offset.x = 0.0f;
    scissor.offset.y = 0.0f;
    scissor.extent = specification.swapchainExtent;
    vk::PipelineViewportStateCreateInfo viewportState = {};
    viewportState.flags = vk::PipelineViewportStateCreateFlags();
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    pipelineInfo.pViewportState = &viewportState;

    //Rasterizer
    vk::PipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
    rasterizer.depthClampEnable = VK_FALSE; //discard out of bounds fragments, don't clamp them
    rasterizer.rasterizerDiscardEnable = VK_FALSE; //This flag would disable fragment output
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = VK_FALSE; //Depth bias can be useful in shadow maps.
    pipelineInfo.pRasterizationState = &rasterizer;

    //Fragment Shader
    LOG("Create fragment shader module");
    m_shaders.fragment = std::make_unique<Shader>(specification.fragmentFilepath, specification.device);
    vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
    fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
    fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragmentShaderInfo.module = m_shaders.fragment->GetModule();
    fragmentShaderInfo.pName = "main";
    shaderStages.push_back(fragmentShaderInfo);
    //Now both shaders have been made, we can declare them to the pipeline info
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();

    //Multisampling
    vk::PipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    pipelineInfo.pMultisampleState = &multisampling;

    //Color Blend
    vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;
    vk::PipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    pipelineInfo.pColorBlendState = &colorBlending;

    //Pipeline Layout
    LOG("Create Pipeline Layout");
    m_bundle.layout = init_pipeline_layout(specification.device);
    pipelineInfo.layout = m_bundle.layout;

    //Renderpass
    LOG("Create RenderPass");
    m_bundle.renderpass = init_renderpass(specification.device, specification.swapchainImageFormat);
    pipelineInfo.renderPass = m_bundle.renderpass;
    pipelineInfo.subpass = 0;

    //Extra stuff
    pipelineInfo.basePipelineHandle = nullptr;

    //Make the Pipeline
    LOG("Create Graphics Pipeline");
    vk::Pipeline graphicsPipeline;
    try {
        m_bundle.pipeline = (specification.device.createGraphicsPipeline(nullptr, pipelineInfo)).value;
    }
    catch (vk::SystemError err) {
        LOG("Failed to create Pipeline");
    }

    //Finally clean up by destroying shader modules
    specification.device.destroyShaderModule(m_shaders.fragment->GetModule());
    specification.device.destroyShaderModule(m_shaders.vertex->GetModule());
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline::GraphicsPipelineInBundle &specification) :
    m_device(specification.device)
{
    init_graphics_pipeline(specification);
}

GraphicsPipeline::~GraphicsPipeline() {
    m_device.destroyPipeline(m_bundle.pipeline);
    m_device.destroyPipelineLayout(m_bundle.layout);
    m_device.destroyRenderPass(m_bundle.renderpass);
}
