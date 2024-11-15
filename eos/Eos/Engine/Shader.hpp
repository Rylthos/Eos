#pragma once

#include "Eos/EosPCH.hpp"
#include "Eos/Core/DeletionQueue.hpp"

#include <vulkan/vulkan.h>

namespace Eos
{
    class EOS_API Shader
    {
    public:
    public:
        ~Shader();

        void addShaderModule(VkShaderStageFlagBits stage, const char* path);
        void clearModules();

        std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages();
    private:
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
        DeletionQueue m_DeletionQueue;
    private:
        void addShaderStage(VkShaderStageFlagBits stage, VkShaderModule& shaderModule);
    };
}
