#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Engine/GlobalData.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Eos
{
    class EOS_API Buffer
    {
    public:
        VkBuffer buffer;
        VmaAllocation allocation;
    public:
        Buffer() {}

        void create(size_t allocSize, VkBufferUsageFlags usage,
                VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags = 0);

        void create(size_t allocSize, VkBufferUsageFlags usage,
                VkSharingMode sharingMode, std::vector<uint32_t> queues,
                VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags = 0);

        void addToDeletionQueue(DeletionQueue& deletionQueue);
    };
}
