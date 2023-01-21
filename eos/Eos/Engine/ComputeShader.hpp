#pragma once

#include "Eos/EosPCH.hpp"
#include <vulkan/vulkan.hpp>

namespace Eos
{
    class EOS_API ComputeShader
    {
    public:
        ComputeShader();
        ~ComputeShader();

        static void setup(Queue* queue);
        static void cleanup();

        static void end(VkCommandBuffer& cmd);
        static void wait(VkCommandBuffer& cmd);
        static void endAndWait(VkCommandBuffer& cmd);

        static void resetCommandBuffer(VkCommandBuffer& cmd);

        void clearModule();
        void addShaderModule(const char* path);

        static VkCommandBuffer getCommandBuffer();
        static VkFence getFence() { return s_Fence; }
        static Queue* getQueue() { return s_Queue; }

        VkPipelineShaderStageCreateInfo& getShaderStage() { return m_ShaderStage; }

    private:
        VkPipelineShaderStageCreateInfo m_ShaderStage;
        VkShaderModule m_Module;

        bool m_CreatedModule = false;

        static VkCommandPool s_CommandPool;
        static VkFence s_Fence;

        static Queue* s_Queue;

        static void initData();
    };
}
