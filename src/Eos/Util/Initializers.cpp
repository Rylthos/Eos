#include "Initializers.hpp"
#include <cstdlib>

namespace Eos::Init
{
    VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t queueFamilyIndex,
            VkCommandPoolCreateFlags flags)
    {
        VkCommandPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.queueFamilyIndex = queueFamilyIndex;
        info.flags = flags;

        return info;
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool pool,
            uint32_t count, VkCommandBufferLevel level)
    {
        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.commandPool = pool;
        info.commandBufferCount = count;
        info.level = level;

        return info;
    }

    VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags)
    {
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;

        return info;
    }

    VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags)
    {
        VkSemaphoreCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;

        return info;
    }
}
