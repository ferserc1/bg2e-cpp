
#include <bg2e/render/vulkan/Pipeline.hpp>

#include <bg2e/render/vulkan/VulkanAPI.hpp>

namespace bg2e {
namespace render {
namespace vulkan {

Pipeline::Pipeline()
{
    // TODO: Initialize structures with default values
    // Default inputAssembly options
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = false;
    
    // Default rasterizer options
    rasterizer.depthClampEnable = false;
    rasterizer.rasterizerDiscardEnable = false;  // Si es true, el fragmento nunca llega al fragment shader
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = false;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
}

Pipeline::Pipeline(const Pipeline* pipeline)
{
    // TODO: Initialize _createInfo with pipeline->_createInfo
}

void Pipeline::build(VulkanAPI *vulkanApi)
{
    _device = vulkanApi->device();

    if (!_pipelineLayout)
    {
        throw std::runtime_error("Failed to create pipeline: invalid pipeline layout set");
    }

    // Shaders

    // Dynamic state
    vk::PipelineDynamicStateCreateInfo dynamicState;
    if (dynamicStates.size() > 0)
    {
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();
    }
    _createInfo.pDynamicState = &dynamicState;
    

    // Vertex input
    // TODO

    // Input assembly
    _createInfo.pInputAssemblyState = &inputAssembly;

    // Viewport anmd scissor
    // TODO

    // Rasterizator
    _createInfo.pRasterizationState = &rasterizer;

    // Multisample
    // TODO

    // Color blend
    if (_useDefaultColorBlend)
    {
        // Use default color blend
        /*
        * vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = false;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional
        
        vk::PipelineColorBlendStateCreateInfo colorBlending;
        colorBlending.logicOpEnable = false;
        colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional
        */
    }
    _createInfo.pColorBlendState = &_colorBlendCreateInfo;


    // Pipeline layout
    _createInfo.layout = _pipelineLayout->impl();

    // TODO: Support custom render pass and subpasses
    _createInfo.renderPass = vulkanApi->mainRenderPass();
    _createInfo.subpass = 0;

    _createInfo.basePipelineHandle = nullptr;
    _createInfo.basePipelineIndex = -1;

    // Create pipeline
    vk::Result result;
    std::tie(result, _pipeline) = _device.createGraphicsPipeline(nullptr, _createInfo);
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    // Destroy shader modules
}

void Pipeline::destroy()
{
    _device.destroyPipeline(_pipeline);
    _pipeline = nullptr;
}

}
}
}
