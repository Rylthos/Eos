#pragma once

#include "Eos/EosPCH.hpp"
#include "Eos/Core/Window.hpp"

#include "Eos/Core/DeletionQueue.hpp"

#include "Eos/Engine/Mesh.hpp"
#include "Eos/Engine/Pipelines/PipelineBuilder.hpp"
#include "Eos/Engine/Shader.hpp"
#include "Eos/Engine/Texture.hpp"

#include "Eos/Engine/DescriptorSets/DescriptorAllocator.hpp"
#include "Eos/Engine/DescriptorSets/DescriptorLayoutCache.hpp"
#include "Eos/Engine/DescriptorSets/DescriptorBuilder.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Eos
{
    struct EngineSetupDetails
    {
        const char* name;
        bool vsync;
    };

    struct FrameData
    {
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;

        VkFence renderFence;
        VkSemaphore renderSemaphore, presentSemaphore;
    };

    struct RenderInformation
    {
        FrameData* frame;
        uint32_t swapchainImageIndex;

        VkCommandBuffer* cmd;
    };

    struct UploadContext
    {
        VkFence uploadFence;
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
    };

    class EOS_API Engine
    {
    public:
        static Engine* get();

        PipelineBuilder createPipelineBuilder();
        DescriptorBuilder createDescriptorBuilder();

        void cleanup();

        void init(Window& window, const EngineSetupDetails& setupDetails);

        RenderInformation preRender(int frameNumber);
        void postRender(RenderInformation& information);

        Engine(const Engine&) = delete;
        void operator=(const Engine&) = delete;
    private:
        bool m_Initialized = false;
        VkExtent2D m_WindowExtent;
        uint32_t m_FrameOverlap = 2;

        EngineSetupDetails m_SetupDetails;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkSurfaceKHR m_Surface;

        VmaAllocator m_Allocator;

        Queue m_GraphicsQueue;
        Queue m_TransferQueue;

        Swapchain m_Swapchain;

        VkRenderPass m_Renderpass;
        std::vector<VkFramebuffer> m_Framebuffers;

        VkFormat m_DepthFormat = VK_FORMAT_D32_SFLOAT;
        Texture2D m_DepthImage;

        std::vector<FrameData> m_Frames;

        UploadContext m_UploadContext;

        DescriptorAllocator m_DescriptorAllocator;
        DescriptorLayoutCache m_DescriptorLayoutCache;

        DeletionQueue m_DeletionQueue;

    private:
        Engine() {}
        ~Engine() {}

        void initVulkan(Window& window, const char* name);
        void initSwapchain();
        void initRenderpass();
        void initFramebuffers();
        void initCommands();
        void initSyncStructures();
        void initDescriptorSets();
    };
}
