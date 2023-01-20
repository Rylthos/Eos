#include "RenderPassBuilder.hpp"

#include "Eos/Engine/GlobalData.hpp"
#include <vulkan/vulkan_core.h>

namespace Eos
{
    RenderPassBuilder RenderPassBuilder::begin(RenderPass& renderpass)
    {
        return RenderPassBuilder(renderpass);
    }

    RenderPassBuilder& RenderPassBuilder::addDefaultDepthBuffer(int width, int height)
    {
        m_RenderPass.depthImageFormat = std::make_optional(VK_FORMAT_D32_SFLOAT);
        m_DepthImageExtent = VkExtent3D {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height), 1 };

        m_RenderPass.depthImage = std::make_optional(Texture2D{});

        m_RenderPass.depthImage->createImage(*m_RenderPass.depthImageFormat,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_DepthImageExtent,
                VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        m_RenderPass.depthImage->createImageView(*m_RenderPass.depthImageFormat,
                VK_IMAGE_ASPECT_DEPTH_BIT);
        m_RenderPass.depthImage->addToDeletionQueue(GlobalData::getDeletionQueue());

        VkAttachmentDescription attachment{};
        attachment.format = *m_RenderPass.depthImageFormat;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference reference{};
        reference.attachment = m_AttachmentDescriptions.size();
        reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        m_AttachmentDescriptions.push_back(attachment);
        m_SubpassDependencies.push_back(dependency);

        m_DepthAttachmentReference = std::make_optional(reference);

        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::addAttachment(VkAttachmentDescription& description,
            VkImageLayout layout, VkSubpassDependency& dependency)
    {
        VkAttachmentReference attachmentReference{};
        attachmentReference.layout = layout;
        attachmentReference.attachment = m_AttachmentDescriptions.size();

        m_AttachmentDescriptions.push_back(description);
        m_AttachmentReferences.push_back(attachmentReference);
        m_SubpassDependencies.push_back(dependency);

        return *this;
    }

    bool RenderPassBuilder::build()
    {
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = static_cast<uint32_t>(m_AttachmentReferences.size());
        subpass.pColorAttachments = m_AttachmentReferences.data();
        subpass.pDepthStencilAttachment = nullptr;

        if (m_DepthAttachmentReference.has_value())
            subpass.pDepthStencilAttachment = &m_DepthAttachmentReference.value();

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(m_AttachmentDescriptions.size());
        renderPassInfo.pAttachments = m_AttachmentDescriptions.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(m_SubpassDependencies.size());
        renderPassInfo.pDependencies = m_SubpassDependencies.data();

        EOS_VK_CHECK(vkCreateRenderPass(GlobalData::getDevice(), &renderPassInfo, nullptr,
                    &m_RenderPass.renderPass));

        VkRenderPass rp = m_RenderPass.renderPass;

        GlobalData::getDeletionQueue().pushFunction([=]()
                { vkDestroyRenderPass(GlobalData::getDevice(), rp, nullptr); });

        EOS_LOG_INFO("Created Render Pass");

        return true;
    }
}
