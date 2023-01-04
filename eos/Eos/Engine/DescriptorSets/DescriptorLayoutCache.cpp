#include "DescriptorLayoutCache.hpp"

#include <algorithm>

namespace Eos
{
    bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(
            const DescriptorLayoutInfo& other) const
    {
        if (other.bindings.size() != bindings.size())
            return false;

        for (size_t i = 0; i < bindings.size(); i++)
        {
            if (other.bindings[i].binding != bindings[i].binding)
                return false;

            if (other.bindings[i].descriptorType != bindings[i].descriptorType)
                return false;

            if (other.bindings[i].descriptorCount != bindings[i].descriptorCount)
                return false;

            if (other.bindings[i].stageFlags != bindings[i].stageFlags)
                return false;
        }

        return true;
    }

    size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const
    {
        size_t result = std::hash<size_t>()(bindings.size());

        for (const VkDescriptorSetLayoutBinding& b : bindings)
        {
            size_t bindingHash = b.binding | b.descriptorType << 8 |
                b.descriptorCount << 16 | b.stageFlags << 24;
            result ^= std::hash<size_t>()(bindingHash);
        }

        return result;
    }

    void DescriptorLayoutCache::init(VkDevice newDevice)
    {
        m_Device = newDevice;
    }

    void DescriptorLayoutCache::cleanup()
    {
        for (auto pair : m_LayoutCache)
        {
            vkDestroyDescriptorSetLayout(m_Device, pair.second, nullptr);
        }
    }

    VkDescriptorSetLayout DescriptorLayoutCache::createDescriptorLayout(
            VkDescriptorSetLayoutCreateInfo* info)
    {
        DescriptorLayoutInfo layoutInfo;
        layoutInfo.bindings.reserve(info->bindingCount);
        bool isSorted = true;
        int lastBinding = -1;

        for (uint32_t i = 0; i < info->bindingCount; i++)
        {
            layoutInfo.bindings.push_back(info->pBindings[i]);

            if (info->pBindings[i].binding > lastBinding)
            {
                lastBinding = info->pBindings[i].binding;
            }
            else
            {
                isSorted = false;
            }
        }

        if (!isSorted)
        {
            std::sort(layoutInfo.bindings.begin(), layoutInfo.bindings.end(),
                    [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b)
                    {
                        return a.binding < b.binding;
                    });
        }

        auto it = m_LayoutCache.find(layoutInfo);
        if (it != m_LayoutCache.end())
        {
            return (*it).second;
        }
        else
        {
            VkDescriptorSetLayout layout;
            vkCreateDescriptorSetLayout(m_Device, info, nullptr, &layout);

            m_LayoutCache[layoutInfo] = layout;
            return layout;
        }
    }
}
