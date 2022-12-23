#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Util/PipelineCreation.hpp"

#include <Vulkan/Vulkan.h>

namespace Eos
{
    struct VertexInputDescription;

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

        void addVertexInputInfo(const VertexInputDescription& description);
    private:
        VertexInputDescription m_VertexDescription;
    };
}
