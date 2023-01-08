#include "PipelineCreation.hpp"

namespace Eos::Pipeline
{
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo()
    {
        VkPipelineVertexInputStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.vertexBindingDescriptionCount = 0;
        info.vertexAttributeDescriptionCount = 0;

        return info;
    }

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo(
            VkPrimitiveTopology topology)
    {
        VkPipelineInputAssemblyStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.topology = topology;
        info.primitiveRestartEnable = VK_FALSE;

        return info;
    }

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo(
            VkPolygonMode polygonMode)
    {
        VkPipelineRasterizationStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.depthClampEnable = VK_FALSE;
        info.rasterizerDiscardEnable = VK_FALSE;
        info.polygonMode = polygonMode;
        info.lineWidth = 1.0f;
        info.cullMode = VK_CULL_MODE_NONE;
        info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        info.depthBiasEnable = VK_FALSE;
        info.depthBiasConstantFactor = 0.0f;
        info.depthBiasClamp = 0.0f;
        info.depthBiasSlopeFactor = 0.0f;

        return info;
    }

    VkPipelineMultisampleStateCreateInfo multisamplingStateCreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.sampleShadingEnable = VK_FALSE;
        info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        info.minSampleShading = 1.0f;
        info.pSampleMask = nullptr;
        info.alphaToCoverageEnable = VK_FALSE;
        info.alphaToOneEnable = VK_FALSE;

        return info;
    }

    VkPipelineColorBlendAttachmentState colourBlendAttachmentState()
    {
        VkPipelineColorBlendAttachmentState info{};
        info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        info.blendEnable = VK_FALSE;

        return info;
    }

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo(
            bool depthTest, bool depthWrite, VkCompareOp compareOp)
    {
        VkPipelineDepthStencilStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.depthTestEnable = depthTest ? VK_TRUE : VK_FALSE;
        info.depthWriteEnable = depthWrite ? VK_TRUE : VK_FALSE;
        info.depthCompareOp = depthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
        info.depthBoundsTestEnable = VK_FALSE;
        info.minDepthBounds = 0.0f;
        info.maxDepthBounds = 1.0f;
        info.stencilTestEnable = VK_FALSE;

        return info;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo()
    {
        VkPipelineLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.setLayoutCount = 0;
        info.pSetLayouts = nullptr;
        info.pushConstantRangeCount = 0;
        info.pPushConstantRanges = nullptr;

        return info;
    }
}