#pragma once

#include "Eos/EosPCH.hpp"

#include <vulkan/vulkan.hpp>

namespace Eos
{
    class EOS_API Init
    {
    public:
        static VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t queueFamilyIndex,
                VkCommandPoolCreateFlags flags = 0);

        static VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool pool,
                uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        static VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);

        static VkSubmitInfo submitInfo(VkCommandBuffer* cmd);

        static VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0);

        static VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);
    private:
        Init() {}
    };
}
