#include "Buffer.hpp"

#include "Eos/Engine/GlobalData.hpp"

namespace Eos
{
    void Buffer::destroy()
    {
        if (!m_AddedToQueue)
        {
            vmaDestroyBuffer(GlobalData::getAllocator(), buffer, allocation);
        }
    }

    void Buffer::create(size_t allocSize, VkBufferUsageFlags usage,
            VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags)
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

    void Buffer::create(size_t allocSize, VkBufferUsageFlags usage,
            VkSharingMode sharingMode, std::vector<uint32_t> queues,
            VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags)
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

    void Buffer::addToDeletionQueue(DeletionQueue& deletionQueue)
    {
        m_AddedToQueue = true;
        deletionQueue.pushFunction([=]() {
                vmaDestroyBuffer(GlobalData::getAllocator(),
                        buffer, allocation);
                });
    }
}
