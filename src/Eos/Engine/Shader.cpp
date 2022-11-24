#include "Shader.hpp"

#include <fstream>

#include "Engine.hpp"

namespace Eos
{
    Shader::~Shader()
    {
        clearModules();
    }

    void Shader::addShaderModule(VkShaderStageFlagBits stage, const char* path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            exit(-1);

        size_t fileSize = (size_t)file.tellg();

        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

        file.seekg(0);
        file.read((char*)buffer.data(), fileSize);

        file.close();

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.codeSize = buffer.size() * sizeof(uint32_t);
        createInfo.pCode = buffer.data();

        VkShaderModule module;
        if (vkCreateShaderModule(*Engine::get()->getDevice(), &createInfo, nullptr,
                    &module) != VK_SUCCESS)
        {
            exit(-1);
        }

        addShaderStage(stage, module);

        m_DeletionQueue.pushFunction([=]()
                { vkDestroyShaderModule(*Engine::get()->getDevice(), module, nullptr); });
    }

    void Shader::clearModules()
    {
        m_DeletionQueue.flush();
    }

    std::vector<VkPipelineShaderStageCreateInfo>& Shader::getShaderStages()
    {
        return m_ShaderStages;
    }

    void Shader::addShaderStage(VkShaderStageFlagBits stage, VkShaderModule& shaderModule)
    {
        VkPipelineShaderStageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.pNext = nullptr;
        info.stage = stage;
        info.module = shaderModule;
        info.pName = "main";

        m_ShaderStages.push_back(info);
    }
}
