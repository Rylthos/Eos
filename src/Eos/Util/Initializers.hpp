#pragma once

#include <Vulkan/Vulkan.hpp>

namespace Eos::Init
{
    VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t queueFamilyIndex,
            VkCommandPoolCreateFlags flags = 0);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool pool,
            uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
    VkSubmitInfo submitInfo(VkCommandBuffer* cmd);

    VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0);

    VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);
}
