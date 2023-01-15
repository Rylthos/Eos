#pragma once

#include "Eos/EosPCH.hpp"

#include <vulkan/vulkan.h>

namespace Eos
{
    class EOS_API Pipeline
    {
    public:
        static VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo();

        static VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo(
                VkPrimitiveTopology topology);

        static VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo(
                VkPolygonMode polygonMode);

        static VkPipelineMultisampleStateCreateInfo multisamplingStateCreateInfo();

        static VkPipelineColorBlendStateCreateInfo colourBlendStateCreateInfo();
        static VkPipelineColorBlendAttachmentState colourBlendAttachmentState();

        static VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo(
                bool depthTest, bool depthWrite, VkCompareOp compareOp);

        static VkPipelineViewportStateCreateInfo viewportStateCreateInfo();

        static VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo();
    };
}
