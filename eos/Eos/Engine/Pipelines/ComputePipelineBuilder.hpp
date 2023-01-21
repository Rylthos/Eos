#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Engine/Pipelines/PipelineCreationInfo.hpp"

#include "Eos/Core/DeletionQueue.hpp"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Eos
{
    class EOS_API ComputePipelineBuilder
    {
    public:
        static ComputePipelineBuilder begin(VkDevice* device);
        static void cleanup();

        ComputePipelineBuilder& setShaderStage(VkPipelineShaderStageCreateInfo& stage);
        ComputePipelineBuilder& setFlags(VkPipelineCreateFlags flags);

        ComputePipelineBuilder& createPipelineLayout(VkPipelineLayout& layout);
        ComputePipelineBuilder& createPipelineLayout(VkPipelineLayout& layout, const VkPipelineLayoutCreateInfo& layoutCI);

        bool build(VkPipeline& pipeline);
        bool build(VkPipeline& pipeline, const VkPipelineLayout& layout);
        bool build(VkPipeline& pipeline, VkPipelineLayout& layout, const VkPipelineLayoutCreateInfo& layoutCI);
    private:
        VkPipelineShaderStageCreateInfo m_ShaderStage;
        VkPipelineCreateFlags m_Flags;

        VkDevice* m_Device;

        static DeletionQueue s_DeletionQueue;
    };
}
