#pragma once

#include <Vulkan/Vulkan.h>

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
}
