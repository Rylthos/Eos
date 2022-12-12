#pragma once

#include "../../Core/Core.hpp"
#include "DescriptorLayoutCache.hpp"
#include "DescriptorAllocator.hpp"

#include <Vulkan/vulkan.h>

namespace Eos
{
    class EOS_API DescriptorBuilder
    {
    public:
        static DescriptorBuilder begin(DescriptorLayoutCache* layoutCache,
                DescriptorAllocator* allocator);
        DescriptorBuilder& bindBuffer(uint32_t binding,
                VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type,
                VkShaderStageFlags stageFlags);
        DescriptorBuilder& bindImage(uint32_t binding,
                VkDescriptorImageInfo* imageInfo, VkDescriptorType type,
                VkShaderStageFlags stageFlags);

        bool build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);
        bool build(VkDescriptorSet& set);

    private:
        std::vector<VkWriteDescriptorSet> m_Writes;
        std::vector<VkDescriptorSetLayoutBinding> m_Bindings;

        DescriptorLayoutCache* m_Cache;
        DescriptorAllocator* m_Alloc;
    };
}
