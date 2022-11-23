#pragma once

#include <Vulkan/Vulkan.h>

namespace Eos::Pipeline
{
    VkPipelineVertexInputStateCreateInfo   vertexInputStateCreateInfo();
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo(
            VkPrimitiveTopology topology);
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo(
            VkPolygonMode polygonMode);
    VkPipelineMultisampleStateCreateInfo   multisamplingStateCreateInfo();
    VkPipelineColorBlendAttachmentState    colourBlendAttachmentState();
    VkPipelineDepthStencilStateCreateInfo  depthStencilCreateInfo(
            bool depthTest, bool depthWrite, VkCompareOp compareOp);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo();
}
