#pragma once

#include "../../Core/Core.hpp"

#include <Vulkan/vulkan.h>

#include <vector>
#include <unordered_map>

namespace Eos
{
    class EOS_API DescriptorLayoutCache
    {
    public:
        struct DescriptorLayoutInfo
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings;

            bool operator==(const DescriptorLayoutInfo& other) const;
            size_t hash() const;
        };

    public:
        void init(VkDevice newDevice);
        void cleanup();

        VkDescriptorSetLayout createDescriptorLayout(VkDescriptorSetLayoutCreateInfo* info);

    private:
        struct DescriptorLayoutHash
        {
            std::size_t operator()(const DescriptorLayoutInfo& info) const
            {
                return info.hash();
            }
        };

        std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash>
            m_LayoutCache;
        VkDevice m_Device;
    };
}
