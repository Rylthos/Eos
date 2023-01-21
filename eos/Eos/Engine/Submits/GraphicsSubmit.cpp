#include "GraphicsSubmit.hpp"

#include "Eos/Engine/Initializers.hpp"
#include "Eos/Engine/GlobalData.hpp"

namespace Eos
{
    UploadContext GraphicsSubmit::s_UploadContext;
    Queue* GraphicsSubmit::s_GraphicsQueue;

    void GraphicsSubmit::setup(Queue* queue)
    {
        s_GraphicsQueue = queue;

        // Pool
        VkCommandPoolCreateInfo uploadCommandPoolInfo = Init::commandPoolCreateInfo(
                s_GraphicsQueue->family);
        EOS_VK_CHECK(vkCreateCommandPool(GlobalData::getDevice(),
                    &uploadCommandPoolInfo, nullptr, &s_UploadContext.commandPool));

        // Buffer
        VkCommandBufferAllocateInfo cmdAllocInfo = Init::commandBufferAllocateInfo(
                s_UploadContext.commandPool, 1);

        EOS_VK_CHECK(vkAllocateCommandBuffers(GlobalData::getDevice(), &cmdAllocInfo,
                    &s_UploadContext.commandBuffer));

        // Fence
        VkFenceCreateInfo uploadFenceCreateInfo = Init::fenceCreateInfo();
        EOS_VK_CHECK(vkCreateFence(GlobalData::getDevice(), &uploadFenceCreateInfo, nullptr,
                    &s_UploadContext.fence));

        GlobalData::getDeletionQueue().pushFunction([=]() {
                vkDestroyCommandPool(GlobalData::getDevice(), s_UploadContext.commandPool, nullptr);
                vkDestroyFence(GlobalData::getDevice(), s_UploadContext.fence, nullptr);
            });

        EOS_LOG_INFO("Created Upload Context (Graphics Submit)");
    }

    void GraphicsSubmit::submit(std::function<void(VkCommandBuffer)>&& function)
    {
        VkCommandBuffer cmd = s_UploadContext.commandBuffer;

        VkCommandBufferBeginInfo cmdBeginInfo = Init::commandBufferBeginInfo(
                VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        
        EOS_VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        function(cmd);

        EOS_VK_CHECK(vkEndCommandBuffer(cmd));

        VkSubmitInfo submit = Init::submitInfo(&cmd);

        EOS_VK_CHECK(vkQueueSubmit(s_GraphicsQueue->queue, 1, &submit,
                    s_UploadContext.fence));

        vkWaitForFences(GlobalData::getDevice(), 1,
                &s_UploadContext.fence, true, 9999999999);
        vkResetFences(GlobalData::getDevice(), 1, &s_UploadContext.fence);

        vkResetCommandPool(GlobalData::getDevice(), s_UploadContext.commandPool, 0);
    }
}
