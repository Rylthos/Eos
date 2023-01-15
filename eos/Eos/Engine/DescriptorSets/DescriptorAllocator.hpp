#pragma once

// https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/

#include "Eos/EosPCH.hpp"

#include <vulkan/vulkan.h>

namespace Eos
{
    class EOS_API DescriptorAllocator
    {
    public:
        struct PoolSizes
        {
            std::vector<std::pair<VkDescriptorType, float>> sizes =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.0f },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.0f },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.0f },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.0f },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.0f },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.0f },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.0f },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.0f },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
            };
        };

        VkDevice device;
    public:
        void init(VkDevice newDevice);
        void cleanup();

        bool allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);
        void resetPools();

        static VkDescriptorPool createPool(VkDevice device, const PoolSizes& poolSizes,
                int count, VkDescriptorPoolCreateFlags flags);
    private:
        VkDescriptorPool m_CurrentPool{VK_NULL_HANDLE};
        PoolSizes m_DescriptorSizes;

        std::vector<VkDescriptorPool> m_UsedPools;
        std::vector<VkDescriptorPool> m_FreePools;
    private:
        VkDescriptorPool grabPool();
    };
}
