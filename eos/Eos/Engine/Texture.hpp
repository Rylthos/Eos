#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Engine/GlobalData.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace Eos
{
    class EOS_API Texture2D
    {
    public:
        VkImageView imageView;
        VkImage image;
        VmaAllocation allocation;
    public:
        Texture2D() {}

        void createImage(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent,
                VmaMemoryUsage memoryUsage, VkMemoryPropertyFlags memoryFlags = 0)
        {
            VkImageCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.pNext = nullptr;
            info.imageType = VK_IMAGE_TYPE_2D;
            info.format = format;
            info.extent = extent;
            info.mipLevels = 1;
            info.arrayLayers = 1;
            info.samples = VK_SAMPLE_COUNT_1_BIT;
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.usage = usageFlags;

            VmaAllocationCreateInfo vmaAllocInfo{};
            vmaAllocInfo.usage = memoryUsage;
            vmaAllocInfo.requiredFlags = VkMemoryPropertyFlags(memoryFlags);

            EOS_VK_CHECK(vmaCreateImage(GlobalData::getAllocator(),
                        &info, &vmaAllocInfo, &image,
                        &allocation, nullptr));
        }

        void createImageView(VkFormat format, VkImageAspectFlags flags)
        {
            VkImageViewCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.pNext = nullptr;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.image = image;
            info.format = format;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;
            info.subresourceRange.aspectMask = flags;

            EOS_VK_CHECK(vkCreateImageView(GlobalData::getDevice(),
                        &info, nullptr, &imageView));
        }

        void addToDeletionQueue(DeletionQueue& deletionQueue)
        {
            deletionQueue.pushFunction([&]()
            {
                vkDestroyImageView(GlobalData::getDevice(), imageView, nullptr);
                vmaDestroyImage(GlobalData::getAllocator(), image, allocation);
            });
        }
    };
}
