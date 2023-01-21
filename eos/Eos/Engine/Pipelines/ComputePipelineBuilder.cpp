#include "ComputePipelineBuilder.hpp"
#include <vulkan/vulkan_core.h>

namespace Eos
{
    DeletionQueue ComputePipelineBuilder::s_DeletionQueue;

    ComputePipelineBuilder ComputePipelineBuilder::begin(VkDevice* device)
    {
        ComputePipelineBuilder pipeline;
        pipeline.m_Device = device;

        return pipeline;
    }

    void ComputePipelineBuilder::cleanup()
    {
        s_DeletionQueue.flush();
    }

    ComputePipelineBuilder& ComputePipelineBuilder::setShaderStages(
            const std::vector<VkPipelineShaderStageCreateInfo>& stages)
    {
        m_ShaderStage = stages[0];

        return *this;
    }

    ComputePipelineBuilder& ComputePipelineBuilder::setShaderStage(
            VkPipelineShaderStageCreateInfo& stage)
    {
        m_ShaderStage = stage;

        return *this;
    }

    ComputePipelineBuilder& ComputePipelineBuilder::setFlags(VkPipelineCreateFlags flags)
    {
        m_Flags = flags;

        return *this;
    }

    ComputePipelineBuilder& ComputePipelineBuilder::createPipelineLayout(VkPipelineLayout& layout)
    {
        VkPipelineLayoutCreateInfo createInfo = Pipeline::pipelineLayoutCreateInfo();
        return createPipelineLayout(layout, createInfo);
    }

    ComputePipelineBuilder& ComputePipelineBuilder::createPipelineLayout(VkPipelineLayout& layout,
            const VkPipelineLayoutCreateInfo& layoutCI)
    {
        if (vkCreatePipelineLayout(*m_Device, &layoutCI, nullptr, &layout) != VK_SUCCESS)
        {
            EOS_LOG_CRITICAL("Failed to create Pipeline Layout");
        }

        return *this;
    }

    bool ComputePipelineBuilder::build(VkPipeline& pipeline)
    {
        VkPipelineLayout layout;
        createPipelineLayout(layout);

        return build(pipeline, layout);
    }

    bool ComputePipelineBuilder::build(VkPipeline& pipeline, const VkPipelineLayout& layout)
    {
        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.stage = m_ShaderStage;
        pipelineInfo.layout = layout;

        if (vkCreateComputePipelines(*m_Device, VK_NULL_HANDLE, 1, &pipelineInfo,
                    nullptr, &pipeline) != VK_SUCCESS)
        {
            EOS_LOG_CRITICAL("Failed to create Compute Pipeline");
            return false;
        }

        VkDevice tempDevice = *m_Device;
        s_DeletionQueue.pushFunction([=]() {
            vkDestroyPipelineLayout(tempDevice, layout, nullptr);
            vkDestroyPipeline(tempDevice, pipeline, nullptr);
        });

        EOS_LOG_INFO("Built Compute Pipeline");

        return true;
    }

    bool ComputePipelineBuilder::build(VkPipeline& pipeline, VkPipelineLayout& layout, const VkPipelineLayoutCreateInfo& layoutCI)
    {
        createPipelineLayout(layout, layoutCI);

        return build(pipeline, layout);
    }
}
