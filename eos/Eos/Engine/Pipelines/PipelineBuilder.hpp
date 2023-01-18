#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Engine/Pipelines/PipelineCreationInfo.hpp"

#include "Eos/Core/DeletionQueue.hpp"

#include <vulkan/vulkan.h>

namespace Eos
{
    struct VertexInputDescription;

    class EOS_API PipelineBuilder
    {
    public:
        static PipelineBuilder begin(VkDevice* device, VkRenderPass* renderPass);
        static void cleanup();

        PipelineBuilder& defaultValues();

        PipelineBuilder& setShaderStages(const std::vector<VkPipelineShaderStageCreateInfo>& stages);
        PipelineBuilder& setVertexInputInfo(const VertexInputDescription& description);
        PipelineBuilder& setInputAssembly(const VkPipelineInputAssemblyStateCreateInfo& createInfo);
        PipelineBuilder& setMultisampling(const VkPipelineMultisampleStateCreateInfo& createInfo);
        PipelineBuilder& setRasterizer(const VkPipelineRasterizationStateCreateInfo& createInfo);;
        PipelineBuilder& setColourBlendAttachments(const std::vector<VkPipelineColorBlendAttachmentState>& states);
        PipelineBuilder& setColourBlending(const VkPipelineColorBlendStateCreateInfo& createInfo);
        PipelineBuilder& setDepthStencil(const VkPipelineDepthStencilStateCreateInfo& createInfo);
        PipelineBuilder& setViewportState(const VkPipelineViewportStateCreateInfo& createInfo);
        PipelineBuilder& setViewports(const std::vector<VkViewport>& viewports);
        PipelineBuilder& setScissors(const std::vector<VkRect2D>& scissors);

        PipelineBuilder& createPipelineLayout(VkPipelineLayout& layout);
        PipelineBuilder& createPipelineLayout(VkPipelineLayout& layout, const VkPipelineLayoutCreateInfo& createInfo);

        bool build(VkPipeline& pipeline);
        bool build(VkPipeline& pipeline, const VkPipelineLayout& layout);
        bool build(VkPipeline& pipeline, VkPipelineLayout& layout, const VkPipelineLayoutCreateInfo& createInfo);
    private:
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
        VkPipelineVertexInputStateCreateInfo m_VertexInputInfo{};
        VkPipelineInputAssemblyStateCreateInfo m_InputAssembly{};
        VkPipelineMultisampleStateCreateInfo m_Multisampling{};
        VkPipelineRasterizationStateCreateInfo m_Rasterizer{};
        VkPipelineColorBlendStateCreateInfo m_ColourBlendState{};
        std::vector<VkPipelineColorBlendAttachmentState> m_ColourBlendAttachments;
        VkPipelineDepthStencilStateCreateInfo m_DepthStencil{};
        VkPipelineViewportStateCreateInfo m_ViewportState{};

        std::vector<VkViewport> m_Viewports;
        std::vector<VkRect2D> m_Scissors;

        VkDevice* m_Device;
        VkRenderPass* m_RenderPass;

        VertexInputDescription m_VertexDescription;

        static DeletionQueue s_DeletionQueue;
    };
}
