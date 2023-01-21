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
                VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags = 0)
        {
            VkBufferCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            info.pNext = nullptr;
            info.size = allocSize;
            info.usage = usage;

            VmaAllocationCreateInfo vmaAllocInfo{};
            vmaAllocInfo.usage = memoryUsage;
            vmaAllocInfo.flags = flags;

            EOS_VK_CHECK(vmaCreateBuffer(GlobalData::getAllocator(),
                        &info, &vmaAllocInfo, &buffer,
                        &allocation, nullptr));
        }

        void create(size_t allocSize, VkBufferUsageFlags usage,
                VkSharingMode sharingMode, std::vector<uint32_t> queues,
                VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags = 0)
        {
            VkBufferCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            info.pNext = nullptr;
            info.size = allocSize;
            info.usage = usage;
            info.sharingMode = sharingMode;
            info.queueFamilyIndexCount = static_cast<uint32_t>(queues.size());
            info.pQueueFamilyIndices = queues.data();

            VmaAllocationCreateInfo vmaAllocInfo{};
            vmaAllocInfo.usage = memoryUsage;
            vmaAllocInfo.flags = flags;

            EOS_VK_CHECK(vmaCreateBuffer(GlobalData::getAllocator(),
                        &info, &vmaAllocInfo, &buffer,
                        &allocation, nullptr));
        }

        void addToDeletionQueue(DeletionQueue& deletionQueue)
        {
            deletionQueue.pushFunction([=]() {
                vmaDestroyBuffer(GlobalData::getAllocator(),
                    buffer, allocation);
            });
        }
    };
}
