#include "PipelineBuilder.hpp"

#include "../Util/Types.hpp"

#include <iostream>

namespace Eos
{
    void PipelineBuilder::defaultPipelineValues()
    {
        vertexInputInfo = Pipeline::vertexInputStateCreateInfo();
        inputAssembly = Pipeline::inputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        rasterizer = Pipeline::rasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
        multisampling = Pipeline::multisamplingStateCreateInfo();
        colourBlendAttachment = Pipeline::colourBlendAttachmentState();
        depthStencil = Pipeline::depthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS);

        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = 1.0f;
        viewport.height = 1.0f;

        scissor.offset = { 0, 0 };
        scissor.extent = { 1, 1 };
    }

    VkPipeline PipelineBuilder::buildPipeline(VkDevice device, VkRenderPass renderPass)
    {
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineColorBlendStateCreateInfo colourBlending{};
        colourBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colourBlending.pNext = nullptr;
        colourBlending.logicOpEnable = VK_FALSE;
        colourBlending.logicOp = VK_LOGIC_OP_COPY;
        colourBlending.attachmentCount = 1;
        colourBlending.pAttachments = &colourBlendAttachment;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colourBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        VkPipeline pipeline;
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                    nullptr, &pipeline) != VK_SUCCESS)
        {
            std::cerr << "Failed to create Pipeline\n";
            return VK_NULL_HANDLE;
        }

        return pipeline;
    }

    void PipelineBuilder::addVertexInputInfo(const VertexInputDescription& description)
    {
        vertexInputInfo.vertexAttributeDescriptionCount = description.attributes.size();
        vertexInputInfo.pVertexAttributeDescriptions = description.attributes.data();
        vertexInputInfo.vertexBindingDescriptionCount = description.bindings.size();
        vertexInputInfo.pVertexBindingDescriptions = description.bindings.data();
    }
}
