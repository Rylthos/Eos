#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Engine/GlobalData.hpp"

#include "Eos/Engine/Buffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace Eos
{
    class EOS_API Texture2D
    {
    public:
        VkImage image;
        VkImageView imageView;
        std::optional<VkSampler> sampler;

        VkFormat format;
        VkExtent3D extent;
        VmaAllocation allocation;
    public:
        Texture2D() {}

        void loadFromFile(const char* file);

        void createImage(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent,
                VmaMemoryUsage memoryUsage, VkMemoryPropertyFlags memoryFlags = 0);

        void createImageView(VkImageAspectFlags flags);
        void createImageView(VkFormat format, VkImageAspectFlags flags);

        void createSampler(VkFilter filter, VkSamplerAddressMode addressMode);

        void addToDeletionQueue(DeletionQueue& queue);
        void deleteImage();

        void convertImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout,
                VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage,
                VkPipelineStageFlags dstStage);

        static void blitBetween(
                Texture2D& srcTexture, VkImageLayout srcLayout,
                Texture2D& dstTexture, VkImageLayout dstLayout, VkFilter filter);
    private:
        void createImage(VkImageUsageFlags usageFlags, VmaMemoryUsage memoryUsage,
                VkMemoryPropertyFlags memoryFlags = 0);

        void transferBufferToImage(Buffer& stagingBuffer);
    };
}
