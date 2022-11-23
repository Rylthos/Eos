#pragma once

#include "../Core/Core.hpp"
#include "../Util/PipelineCreation.hpp"

#include <Vulkan/Vulkan.h>

#include <vector>

namespace Eos
{
    class EOS_API PipelineBuilder
    {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineColorBlendAttachmentState colourBlendAttachment;
        VkPipelineDepthStencilStateCreateInfo depthStencil;

        VkViewport viewport;
        VkRect2D scissor;

        VkPipelineLayout pipelineLayout;
    public:
        void defaultPipelineValues();
        VkPipeline buildPipeline(VkDevice device, VkRenderPass renderPass);
    };
}
