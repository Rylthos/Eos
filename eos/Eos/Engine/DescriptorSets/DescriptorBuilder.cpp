#include "DescriptorBuilder.hpp"

namespace Eos
{
    DescriptorBuilder DescriptorBuilder::begin(DescriptorLayoutCache* layoutCache,
            DescriptorAllocator* allocator)
    {
        DescriptorBuilder builder;
        builder.m_Cache = layoutCache;
        builder.m_Alloc = allocator;

        return builder;
    }

    DescriptorBuilder& DescriptorBuilder::bindBuffer(uint32_t binding,
            VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type,
            VkShaderStageFlags stageFlags)
    {
        VkDescriptorSetLayoutBinding newBinding{};
        newBinding.descriptorCount = 1;
        newBinding.descriptorType = type;
        newBinding.pImmutableSamplers = nullptr;
        newBinding.stageFlags = stageFlags;
        newBinding.binding = binding;

        m_Bindings.push_back(newBinding);

        VkWriteDescriptorSet newWrite{};
        newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        newWrite.pNext = nullptr;
        newWrite.descriptorCount = 1;
        newWrite.descriptorType = type;
        newWrite.pBufferInfo = bufferInfo;
        newWrite.dstBinding = binding;

        m_Writes.push_back(newWrite);

        return *this;
    }

    DescriptorBuilder& DescriptorBuilder::bindImage(uint32_t binding,
            VkDescriptorImageInfo* imageInfo, VkDescriptorType type,
            VkShaderStageFlags stageFlags)
    {
        VkDescriptorSetLayoutBinding newBinding{};
        newBinding.descriptorCount = 1;
        newBinding.descriptorType = type;
        newBinding.pImmutableSamplers = nullptr;
        newBinding.stageFlags = stageFlags;
        newBinding.binding = binding;

        m_Bindings.push_back(newBinding);

        VkWriteDescriptorSet newWrite{};
        newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        newWrite.pNext = nullptr;
        newWrite.descriptorCount = 1;
        newWrite.descriptorType = type;
        newWrite.pImageInfo = imageInfo;
        newWrite.dstBinding = binding;

        m_Writes.push_back(newWrite);
        return *this;
    }

    bool DescriptorBuilder::build(VkDescriptorSet& set, VkDescriptorSetLayout& layout)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pNext = nullptr;
        layoutInfo.pBindings = m_Bindings.data();
        layoutInfo.bindingCount = m_Bindings.size();

        layout = m_Cache->createDescriptorLayout(&layoutInfo);

        bool success = m_Alloc->allocate(&set, layout);
        if (!success) { return false; }

        for (VkWriteDescriptorSet& w : m_Writes)
        {
            w.dstSet = set;
        }

        vkUpdateDescriptorSets(m_Alloc->device, m_Writes.size(), m_Writes.data(), 0, nullptr);

        return true;
    }

    bool DescriptorBuilder::build(VkDescriptorSet& set)
    {
        VkDescriptorSetLayout layout;
        return build(set, layout);
    }
}
