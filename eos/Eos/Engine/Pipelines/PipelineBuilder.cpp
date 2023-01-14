#include "PipelineBuilder.hpp"

namespace Eos
{
    /* void PipelineBuilder::defaultPipelineValues() */
    /* { */
    /*     VkViewport viewport; */
    /*     viewport.x = 0.0f; */
    /*     viewport.y = 0.0f; */
    /*     viewport.width = 1.0f; */
    /*     viewport.height = 1.0f; */

    /*     VkRect2D scissor; */
    /*     scissor.offset = { 0, 0 }; */
    /*     scissor.extent = { 1, 1 }; */

    /*     defaultPipelineValues(viewport, scissor); */
    /* } */
    
    /* void PipelineBuilder::defaultPipelineValues(VkViewport viewport, VkRect2D scissor) */
    /* { */
    /*     vertexInputInfo = Pipeline::vertexInputStateCreateInfo(); */
    /*     inputAssembly = Pipeline::inputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST); */
    /*     rasterizer = Pipeline::rasterizationStateCreateInfo(VK_POLYGON_MODE_FILL); */
    /*     multisampling = Pipeline::multisamplingStateCreateInfo(); */
    /*     colourBlendAttachment = Pipeline::colourBlendAttachmentState(); */
    /*     depthStencil = Pipeline::depthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS); */

    /*     this->viewport = viewport; */
    /*     this->scissor = scissor; */
    /* } */

    /* VkPipeline PipelineBuilder::buildPipeline(VkDevice device, VkRenderPass renderPass) */
    /* { */
    /*     VkPipelineViewportStateCreateInfo viewportState{}; */
    /*     viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO; */
    /*     viewportState.pNext = nullptr; */
    /*     viewportState.viewportCount = 1; */
    /*     viewportState.pViewports = &viewport; */
    /*     viewportState.scissorCount = 1; */
    /*     viewportState.pScissors = &scissor; */

    /*     VkPipelineColorBlendStateCreateInfo colourBlending{}; */
    /*     colourBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; */
    /*     colourBlending.pNext = nullptr; */
    /*     colourBlending.logicOpEnable = VK_FALSE; */
    /*     colourBlending.logicOp = VK_LOGIC_OP_COPY; */
    /*     colourBlending.attachmentCount = 1; */
    /*     colourBlending.pAttachments = &colourBlendAttachment; */

    /*     VkGraphicsPipelineCreateInfo pipelineInfo{}; */
    /*     pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO; */
    /*     pipelineInfo.pNext = nullptr; */
    /*     pipelineInfo.stageCount = shaderStages.size(); */
    /*     pipelineInfo.pStages = shaderStages.data(); */
    /*     pipelineInfo.pVertexInputState = &vertexInputInfo; */
    /*     pipelineInfo.pInputAssemblyState = &inputAssembly; */
    /*     pipelineInfo.pViewportState = &viewportState; */
    /*     pipelineInfo.pRasterizationState = &rasterizer; */
    /*     pipelineInfo.pMultisampleState = &multisampling; */
    /*     pipelineInfo.pColorBlendState = &colourBlending; */
    /*     pipelineInfo.pDepthStencilState = &depthStencil; */
    /*     pipelineInfo.layout = pipelineLayout; */
    /*     pipelineInfo.renderPass = renderPass; */
    /*     pipelineInfo.subpass = 0; */
    /*     pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; */

    /*     VkPipeline pipeline; */
    /*     if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, */
    /*                 nullptr, &pipeline) != VK_SUCCESS) */
    /*     { */
    /*         EOS_LOG_CRITICAL("Failed to create Pipeline"); */
    /*         return VK_NULL_HANDLE; */
    /*     } */

    /*     EOS_LOG_INFO("Built Pipeline"); */

    /*     return pipeline; */
    /* } */

    /* void PipelineBuilder::addVertexInputInfo(const VertexInputDescription& description) */
    /* { */
    /*     m_VertexDescription = description; */
    /*     vertexInputInfo.vertexAttributeDescriptionCount = m_VertexDescription.attributes.size(); */
    /*     vertexInputInfo.pVertexAttributeDescriptions = m_VertexDescription.attributes.data(); */
    /*     vertexInputInfo.vertexBindingDescriptionCount = m_VertexDescription.bindings.size(); */
    /*     vertexInputInfo.pVertexBindingDescriptions = m_VertexDescription.bindings.data(); */
    /* } */

    PipelineBuilder PipelineBuilder::begin(VkDevice* device, VkRenderPass* renderPass)
    {
        PipelineBuilder builder;
        builder.m_Device = device;
        builder.m_RenderPass = renderPass;

        return builder;
    }

    PipelineBuilder& PipelineBuilder::defaultValues()
    {
        m_VertexInputInfo = Pipeline::vertexInputStateCreateInfo();
        m_InputAssembly = Pipeline::inputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        m_Rasterizer = Pipeline::rasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
        m_Multisampling = Pipeline::multisamplingStateCreateInfo();
        m_ColourBlendState = Pipeline::colourBlendStateCreateInfo();
        m_DepthStencil = Pipeline::depthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS);
        m_ViewportState = Pipeline::viewportStateCreateInfo();

        m_ColourBlendAttachments = { Pipeline::colourBlendAttachmentState() };
        m_Viewports = { { 0.0f, 0.0f, 1.0f, 1.0f } };
        m_Scissors = { { { 0, 0 }, { 1, 1}  } };

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setShaderStages(
            const std::vector<VkPipelineShaderStageCreateInfo>& stages)
    {
        m_ShaderStages = stages;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setVertexInputInfo(const VertexInputDescription& description)
    {
        m_VertexDescription = description;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setInputAssembly(const VkPipelineInputAssemblyStateCreateInfo& createInfo)
    {
        m_InputAssembly = createInfo;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setMultisampling(const VkPipelineMultisampleStateCreateInfo& createInfo)
    {
        m_Multisampling = createInfo;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setRasterizer(const VkPipelineRasterizationStateCreateInfo& createInfo)
    {
        m_Rasterizer = createInfo;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setColourBlendAttachments(const std::vector<VkPipelineColorBlendAttachmentState>& states)
    {
        m_ColourBlendAttachments = states;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setColourBlending(const VkPipelineColorBlendStateCreateInfo& createInfo)
    {
        m_ColourBlendState = createInfo;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setDepthStencil(const VkPipelineDepthStencilStateCreateInfo& createInfo)
    {
        m_DepthStencil = createInfo;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setViewportState(const VkPipelineViewportStateCreateInfo& createInfo)
    {
        m_ViewportState = createInfo;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setViewports(const std::vector<VkViewport>& viewports)
    {
        m_Viewports = viewports;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::setScissors(const std::vector<VkRect2D>& scissors)
    {
        m_Scissors = scissors;

        return *this;
    }

    PipelineBuilder& PipelineBuilder::createPipelineLayout(VkPipelineLayout& layout)
    {
        VkPipelineLayoutCreateInfo createInfo = Pipeline::pipelineLayoutCreateInfo();
        return createPipelineLayout(layout, createInfo);
    }

    PipelineBuilder& PipelineBuilder::createPipelineLayout(VkPipelineLayout& layout,
            VkPipelineLayoutCreateInfo& createInfo)
    {
        if (vkCreatePipelineLayout(*m_Device, &createInfo, nullptr, &layout) != VK_SUCCESS)
        {
            EOS_LOG_CRITICAL("Failed to create Pipeline Layout");
            return *this;
        }

        return *this;
    }

    bool PipelineBuilder::build(VkPipeline& pipeline)
    {
        VkPipelineLayout layout;
        createPipelineLayout(layout);
        return build(pipeline, layout);
    }

    bool PipelineBuilder::build(VkPipeline& pipeline, const VkPipelineLayout& layout)
    {
        m_ViewportState.viewportCount = static_cast<uint32_t>(m_Viewports.size());
        m_ViewportState.pViewports = m_Viewports.data();
        m_ViewportState.scissorCount = static_cast<uint32_t>(m_Scissors.size());
        m_ViewportState.pScissors = m_Scissors.data();

        m_ColourBlendState.attachmentCount = static_cast<uint32_t>(m_ColourBlendAttachments.size());
        m_ColourBlendState.pAttachments = m_ColourBlendAttachments.data();

        m_VertexInputInfo.vertexAttributeDescriptionCount = m_VertexDescription.attributes.size();
        m_VertexInputInfo.pVertexAttributeDescriptions = m_VertexDescription.attributes.data();
        m_VertexInputInfo.vertexBindingDescriptionCount = m_VertexDescription.bindings.size();
        m_VertexInputInfo.pVertexBindingDescriptions = m_VertexDescription.bindings.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.stageCount = m_ShaderStages.size();
        pipelineInfo.pStages = m_ShaderStages.data();
        pipelineInfo.pVertexInputState = &m_VertexInputInfo;
        pipelineInfo.pInputAssemblyState = &m_InputAssembly;
        pipelineInfo.pViewportState = &m_ViewportState;
        pipelineInfo.pRasterizationState = &m_Rasterizer;
        pipelineInfo.pMultisampleState = &m_Multisampling;
        pipelineInfo.pColorBlendState = &m_ColourBlendState;
        pipelineInfo.pDepthStencilState = &m_DepthStencil;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = *m_RenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(*m_Device, VK_NULL_HANDLE, 1, &pipelineInfo,
                    nullptr, &pipeline) != VK_SUCCESS)
        {
            EOS_LOG_CRITICAL("Failed to create Pipeline");
            return false;
        }

        EOS_LOG_INFO("Built Pipeline");

        return true;
    }
}