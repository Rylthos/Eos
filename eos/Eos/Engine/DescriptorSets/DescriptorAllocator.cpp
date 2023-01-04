#include "DescriptorAllocator.hpp"

namespace Eos
{
    void DescriptorAllocator::init(VkDevice newDevice)
    {
        device = newDevice;
    }

    void DescriptorAllocator::cleanup()
    {
        for (auto pool : m_FreePools)
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }

        for (auto pool : m_UsedPools)
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
    }

    bool DescriptorAllocator::allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout)
    {
        if (m_CurrentPool == VK_NULL_HANDLE)
        {
            m_CurrentPool = grabPool();
            m_UsedPools.push_back(m_CurrentPool);
        }

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.pSetLayouts = &layout;
        allocInfo.descriptorPool = m_CurrentPool;
        allocInfo.descriptorSetCount = 1;

        VkResult allocResult = vkAllocateDescriptorSets(device, &allocInfo, set);
        bool needReallocate = false;

        switch (allocResult)
        {
        case VK_SUCCESS:
            return true;
        case VK_ERROR_FRAGMENTED_POOL:
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            needReallocate = true;
            break;
        default:
            return false;
        }

        if (needReallocate)
        {
            m_CurrentPool = grabPool();
            m_UsedPools.push_back(m_CurrentPool);
            allocResult = vkAllocateDescriptorSets(device, &allocInfo, set);
            return (allocResult == VK_SUCCESS);
        }

        return false;
    }

    void DescriptorAllocator::resetPools()
    {
        for (auto pool : m_UsedPools)
        {
            vkResetDescriptorPool(device, pool, 0);
            m_FreePools.push_back(pool);
        }

        m_UsedPools.clear();
        m_CurrentPool = VK_NULL_HANDLE;
    }

    VkDescriptorPool DescriptorAllocator::createPool(VkDevice device, const PoolSizes& poolSizes,
            int count, VkDescriptorPoolCreateFlags flags)
    {
        std::vector<VkDescriptorPoolSize> sizes;
        sizes.reserve(poolSizes.sizes.size());
        for (auto sz : poolSizes.sizes)
        {
            sizes.push_back({ sz.first, uint32_t(sz.second * count) });
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = flags;
        poolInfo.maxSets = count;
        poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
        poolInfo.pPoolSizes = sizes.data();

        VkDescriptorPool descriptorPool;
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);

        return descriptorPool;
    }

    VkDescriptorPool DescriptorAllocator::grabPool()
    {
        if (m_FreePools.size() > 0)
        {
            VkDescriptorPool pool = m_FreePools.back();
            m_FreePools.pop_back();
            return pool;
        }
        else
        {
            return createPool(device, m_DescriptorSizes, 1000, 0);
        }
    }
}
