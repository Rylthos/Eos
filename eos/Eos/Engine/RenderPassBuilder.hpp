#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Engine/Texture.hpp"

#include <vulkan/vulkan.h>

namespace Eos
{
    class EOS_API RenderPassBuilder
    {
    public:
        static RenderPassBuilder begin(RenderPass& renderpass);

        RenderPassBuilder& addDefaultDepthBuffer(int width, int height);
        
        RenderPassBuilder& addAttachment(VkAttachmentDescription& description,
                VkImageLayout layout, VkSubpassDependency& dependency);

        bool build();
    private:
        RenderPassBuilder(RenderPass& renderPass)
            : m_RenderPass(renderPass) {}

        RenderPass& m_RenderPass;

        std::vector<VkAttachmentDescription> m_AttachmentDescriptions;
        std::vector<VkAttachmentReference> m_AttachmentReferences;
        std::vector<VkSubpassDependency> m_SubpassDependencies;

        VkExtent3D m_DepthImageExtent;
        std::optional<VkAttachmentReference> m_DepthAttachmentReference;
    };
}
