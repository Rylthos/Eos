#pragma once

#include <Vulkan/Vulkan.h>
#include "vk_mem_alloc.h"

#include <vector>

namespace Eos
{
    struct Queue
    {
        VkQueue queue;
        uint32_t family;
    };

    struct Swapchain
    {
        VkSwapchainKHR swapchain;
        VkFormat imageFormat;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
    };

    struct Buffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
    };

    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags flags = 0;
    };
}
