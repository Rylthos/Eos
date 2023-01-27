#include "Texture.hpp"

#include "Eos/Engine/GlobalData.hpp"

#include "Eos/Engine/Submits/GraphicsSubmit.hpp"

namespace Eos
{
    void Texture2D::loadFromFile(const char* file)
    {
        int texWidth;
        int texHeight;
        int texChannels;

        stbi_uc* pixels = stbi_load(file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if (!pixels)
        {
            EOS_CORE_LOG_ERROR("Failed to load Texture {}", file);
            return;
        }

        format = VK_FORMAT_R8G8B8A8_SRGB;
        extent.width = texWidth;
        extent.height = texHeight;
        extent.depth = 1;

        createImage(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                VMA_MEMORY_USAGE_GPU_ONLY);
        createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        createSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);

        size_t totalSize = texWidth * texHeight * texChannels;

        Buffer stagingBuffer;
        stagingBuffer.create(totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VMA_MEMORY_USAGE_CPU_ONLY);

        void* pixelsPtr = pixels;
        void* data;

        vmaMapMemory(GlobalData::getAllocator(), stagingBuffer.allocation, &data);
            memcpy(data, pixelsPtr, totalSize);
        vmaUnmapMemory(GlobalData::getAllocator(), stagingBuffer.allocation);

        transferBufferToImage(stagingBuffer);

        vmaDestroyBuffer(Eos::GlobalData::getAllocator(), stagingBuffer.buffer,
                stagingBuffer.allocation);
    }

    void Texture2D::createImage(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent,
            VmaMemoryUsage memoryUsage, VkMemoryPropertyFlags memoryFlags)
    {
        this->extent = extent;
        this->format = format;

        createImage(usageFlags, memoryUsage, memoryFlags);
    }

    void Texture2D::createImageView(VkImageAspectFlags flags)
    {
        createImageView(format, flags);
    }

    void Texture2D::createImageView(VkFormat format, VkImageAspectFlags flags)
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

    void Texture2D::createSampler(VkFilter filter, VkSamplerAddressMode addressMode)
    {
        VkSamplerCreateInfo samplerCI{};
        samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCI.pNext = nullptr;
        samplerCI.minFilter = filter;
        samplerCI.magFilter = filter;
        samplerCI.addressModeU = addressMode;
        samplerCI.addressModeV = addressMode;
        samplerCI.addressModeW = addressMode;

        VkSampler sampler;
        EOS_VK_CHECK(vkCreateSampler(Eos::GlobalData::getDevice(), &samplerCI, nullptr, &sampler));

        this->sampler = std::make_optional(sampler);
    }

    void Texture2D::addToDeletionQueue(DeletionQueue& queue)
    {
        queue.pushFunction([&]() {
            vkDestroyImageView(GlobalData::getDevice(), imageView, nullptr);
            vmaDestroyImage(GlobalData::getAllocator(), image, allocation);

            if (sampler.has_value())
            {
                vkDestroySampler(GlobalData::getDevice(), sampler.value(), nullptr);
            }
        });
    }

    void Texture2D::deleteImage()
    {
        vkDestroyImageView(GlobalData::getDevice(), imageView, nullptr);
        vmaDestroyImage(GlobalData::getAllocator(), image, allocation);

        if (sampler.has_value())
        {
            vkDestroySampler(GlobalData::getDevice(), sampler.value(), nullptr);
            sampler.reset();
        }
    }

    void Texture2D::convertImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout,
            VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage,
            VkPipelineStageFlags dstStage)
    {
        GraphicsSubmit::submit([&](VkCommandBuffer cmd){
            VkImageSubresourceRange range;
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            VkImageMemoryBarrier imageBarrier{};
            imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageBarrier.oldLayout = oldLayout;
            imageBarrier.newLayout = newLayout;
            imageBarrier.image = image;
            imageBarrier.subresourceRange = range;
            imageBarrier.srcAccessMask = srcAccess;
            imageBarrier.dstAccessMask = dstAccess;

            vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr,
                    1, &imageBarrier);
        });
    }

    void Texture2D::blitBetween(
        Texture2D& srcTexture, VkImageLayout srcLayout,
        Texture2D& dstTexture, VkImageLayout dstLayout, VkFilter filter)
    {
        GraphicsSubmit::submit([&](VkCommandBuffer cmd) {
            VkImageSubresourceLayers src;
            src.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            src.mipLevel = 0;
            src.baseArrayLayer = 0;
            src.layerCount = 1;

            VkOffset3D srcOffsets[2];
            srcOffsets[0] = { 0, 0, 0 };
            srcOffsets[1] = {
                static_cast<int32_t>(srcTexture.extent.width),
                static_cast<int32_t>(srcTexture.extent.height),
                1
            };

            VkImageSubresourceLayers dst;
            dst.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            dst.mipLevel = 0;
            dst.baseArrayLayer = 0;
            dst.layerCount = 1;

            VkOffset3D dstOffsets[2];
            dstOffsets[0] = { 0, 0, 0 };
            dstOffsets[1] = {
                static_cast<int32_t>(dstTexture.extent.width),
                static_cast<int32_t>(dstTexture.extent.height),
                1
            };

            VkImageBlit region{};
            region.srcSubresource = src;
            region.dstSubresource = dst;
            region.srcOffsets[0] = srcOffsets[0];
            region.srcOffsets[1] = srcOffsets[1];
            region.dstOffsets[0] = dstOffsets[0];
            region.dstOffsets[1] = dstOffsets[1];

            vkCmdBlitImage(cmd,
                    srcTexture.image, srcLayout,
                    dstTexture.image, dstLayout,
                    1, &region, filter);
        });
    }

    void Texture2D::createImage(VkImageUsageFlags usageFlags, VmaMemoryUsage memoryUsage,
            VkMemoryPropertyFlags memoryFlags)
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

    void Texture2D::transferBufferToImage(Buffer& stagingBuffer)
    {
        GraphicsSubmit::submit([&](VkCommandBuffer cmd) {
            VkImageSubresourceRange range;
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            VkImageMemoryBarrier imageBarrierToTransfer{};
            imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageBarrierToTransfer.image = image;
            imageBarrierToTransfer.subresourceRange = range;
            imageBarrierToTransfer.srcAccessMask = 0;
            imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
                    0, nullptr, 1, &imageBarrierToTransfer);

            VkBufferImageCopy copyRegion{};
            copyRegion.bufferOffset = 0;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.mipLevel = 0;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageExtent = extent;

            vkCmdCopyBufferToImage(cmd, stagingBuffer.buffer, image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            VkImageMemoryBarrier imageBarrierToReadable = imageBarrierToTransfer;
            imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr,
                    1, &imageBarrierToReadable);
        });
    }
}
