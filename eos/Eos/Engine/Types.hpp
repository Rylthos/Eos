#pragma once

#include "Eos/Engine/Texture.hpp"

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

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
        VkExtent2D extent;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
    };

    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags flags = 0;
    };

    struct RenderPass
    {
        std::optional<Texture2D> depthImage;
        std::optional<VkFormat> depthImageFormat;

        VkRenderPass renderPass;
    };

    struct UploadContext
    {
        VkFence fence;
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
    };
}
