#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Core/Window.hpp"
#include "Eos/Core/DeletionQueue.hpp"

#include "Eos/Engine/DescriptorSets/DescriptorAllocator.hpp"
#include "Eos/Engine/DescriptorSets/DescriptorLayoutCache.hpp"
#include "Eos/Engine/DescriptorSets/DescriptorBuilder.hpp"

#include "Eos/Engine/Pipelines/ComputePipelineBuilder.hpp"
#include "Eos/Engine/Pipelines/PipelineBuilder.hpp"

#include "Eos/Engine/ComputeShader.hpp"
#include "Eos/Engine/Mesh.hpp"
#include "Eos/Engine/RenderPassBuilder.hpp"
#include "Eos/Engine/Shader.hpp"
#include "Eos/Engine/Texture.hpp"

#include "Eos/Engine/Submits/TransferSubmit.hpp"
#include "Eos/Engine/Submits/GraphicsSubmit.hpp"

#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace Eos
{
    struct EngineSetupDetails
    {
        const char* name;
        bool vsync;
        uint32_t framesInFlight;
        bool float64;
        VkSurfaceFormatKHR swapchainFormat;

        std::optional<std::function<void(RenderPass&)>> renderpassCreationFunc;

        std::optional<std::function<std::vector<VkImageView>(VkFramebufferCreateInfo&,
                VkImageView&, RenderPass&)>> framebufferCreationFunc;

        std::optional<std::function<std::vector<VkClearValue>()>> renderClearValues;
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

    class EOS_API Engine
    {
    public:
        static Engine* get();

        PipelineBuilder createPipelineBuilder();
        ComputePipelineBuilder createComputePipelineBuilder();
        DescriptorBuilder createDescriptorBuilder();

        std::shared_ptr<Window>& getWindow() { return m_Window; }
        Swapchain& getSwapchain() { return m_Swapchain; }

        Queue getGraphicsQueue() { return m_GraphicsQueue; }
        Queue getTransferQueue() { return m_TransferQueue; }
        Queue getComputeQueue() { return m_ComputeQueue; }

        void cleanup();

        void init(const EngineSetupDetails& setupDetails);

        RenderInformation preRender();
        void postRender(RenderInformation& information);

        Engine(const Engine&) = delete;
        void operator=(const Engine&) = delete;
    private:
        bool m_Initialized = false;

        std::shared_ptr<Window> m_Window;

        EngineSetupDetails m_SetupDetails;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkSurfaceKHR m_Surface;

        VmaAllocator m_Allocator;

        Queue m_GraphicsQueue;
        Queue m_TransferQueue;
        Queue m_ComputeQueue;

        Swapchain m_Swapchain;

        RenderPass m_Renderpass;
        std::vector<VkFramebuffer> m_Framebuffers;

        std::vector<FrameData> m_Frames;

        UploadContext m_UploadContext;

        DescriptorAllocator m_DescriptorAllocator;
        DescriptorLayoutCache m_DescriptorLayoutCache;

        DeletionQueue m_DeletionQueue;

    private:
        Engine();
        ~Engine() {}

        void initVulkan();
        void initSwapchain();
        void initDefaultRenderpass();
        void initFramebuffers();
        void initCommands();
        void initSyncStructures();
        void initDescriptorSets();

        void initImgui();

        void recreateSwapchain();
    };
}
