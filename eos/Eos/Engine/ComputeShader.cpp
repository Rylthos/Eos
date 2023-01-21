#include "ComputeShader.hpp"

#include <fstream>
#include <vulkan/vulkan_core.h>

#include "Eos/Core/DeletionQueue.hpp"
#include "Eos/Engine/Initializers.hpp"

namespace Eos
{
    VkCommandPool ComputeShader::s_CommandPool;
    VkFence ComputeShader::s_Fence;
    Queue* ComputeShader::s_Queue;

    ComputeShader::ComputeShader()
    {
        
    }

    ComputeShader::~ComputeShader()
    {
        clearModule();
    }

    void ComputeShader::setup(Queue* queue)
    {
        s_Queue = queue;
        initData();
    }

    void ComputeShader::cleanup()
    {
        vkResetCommandPool(GlobalData::getDevice(), s_CommandPool,
                VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        vkDestroyFence(GlobalData::getDevice(), s_Fence, nullptr);
        vkDestroyCommandPool(GlobalData::getDevice(), s_CommandPool, nullptr);
    }

    void ComputeShader::end(VkCommandBuffer& cmd)
    {
        EOS_VK_CHECK(vkEndCommandBuffer(cmd));
    }

    void ComputeShader::wait(VkCommandBuffer& cmd)
    {
        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pNext = nullptr;
        submit.pWaitDstStageMask = nullptr;
        submit.waitSemaphoreCount = 0;
        submit.pWaitSemaphores = nullptr;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;

        EOS_VK_CHECK(vkQueueSubmit(s_Queue->queue, 1, &submit, s_Fence));
        EOS_VK_CHECK(vkWaitForFences(GlobalData::getDevice(), 1, &s_Fence,
                    true, std::uint64_t(-1)));
    }

    void ComputeShader::endAndWait(VkCommandBuffer& cmd)
    {
        end(cmd);
        wait(cmd);
    }

    void ComputeShader::resetCommandBuffer(VkCommandBuffer& cmd)
    {
        vkResetCommandBuffer(cmd, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }

    void ComputeShader::clearModule()
    {
        if (!m_CreatedModule) return;

        vkDestroyShaderModule(GlobalData::getDevice(), m_Module, nullptr);
        m_CreatedModule = false;
    }

    void ComputeShader::addShaderModule(const char* path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            EOS_LOG_CRITICAL("Could not find shader '{}'", path);
        }

        size_t filesize = (size_t)file.tellg();

        std::vector<uint32_t> buffer(filesize / sizeof(uint32_t));

        file.seekg(0);
        file.read((char*)buffer.data(), filesize);

        file.close();

        VkShaderModuleCreateInfo moduleCI{};
        moduleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCI.pNext = nullptr;
        moduleCI.codeSize = buffer.size() * sizeof(uint32_t);
        moduleCI.pCode = buffer.data();

        if (vkCreateShaderModule(GlobalData::getDevice(), &moduleCI, nullptr,
                    &m_Module))
        {
            EOS_LOG_CRITICAL("Failed to create Shader Module");
        }

        m_ShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStage.pNext = nullptr;
        m_ShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        m_ShaderStage.module = m_Module;
        m_ShaderStage.pName = "main";

        m_CreatedModule = true;
    }

    VkCommandBuffer ComputeShader::getCommandBuffer()
    {
        VkCommandBuffer cmd;

        VkCommandBufferAllocateInfo cmdAI = Init::commandBufferAllocateInfo(
                s_CommandPool, 1);

        EOS_VK_CHECK(vkAllocateCommandBuffers(GlobalData::getDevice(), &cmdAI,
                    &cmd));

        VkCommandBufferBeginInfo cmdBeginInfo{};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;
        cmdBeginInfo.pInheritanceInfo = nullptr;
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        EOS_VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        return cmd;
    }

    void ComputeShader::initData()
    {
        VkCommandPoolCreateInfo commandPoolCI = Init::commandPoolCreateInfo(
                s_Queue->family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        EOS_VK_CHECK(vkCreateCommandPool(GlobalData::getDevice(), &commandPoolCI,
                    nullptr, &s_CommandPool));

        VkFenceCreateInfo fenceCI = Init::fenceCreateInfo();

        EOS_VK_CHECK(vkCreateFence(GlobalData::getDevice(), &fenceCI, nullptr,
                    &s_Fence));
    }
}
