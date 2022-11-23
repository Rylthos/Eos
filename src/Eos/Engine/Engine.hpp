#pragma once

#include "../Core/Core.hpp"

#include "../Util/DeletionQueue.hpp"
#include "../Util/Types.hpp"
#include "PipelineBuilder.hpp"
#include "Shader.hpp"

#include <cstdint>

#include <GLFW/glfw3.h>
#include <Vulkan/Vulkan.h>
#include <vk_mem_alloc.h>

namespace Eos
{
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
        static Engine& get();
        DeletionQueue& getDeletionQueue();
        PipelineBuilder& getPipelineBuilder();
        VkDevice& getDevice();

        void cleanup();
        void init(GLFWwindow* window, const char* name);

        VkPipelineLayout setupPipelineLayout();
        VkPipeline setupPipeline(VkPipelineLayout layout);

        RenderInformation preRender(int frameNumber);
        void postRender(RenderInformation information);

    private:
        bool m_Initialized = false;
        VkExtent2D m_WindowExtent;
        uint32_t m_FrameOverlap = 2;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkSurfaceKHR m_Surface;

        VmaAllocator m_Allocator;

        Queue m_GraphicsQueue;
        Swapchain m_Swapchain;

        VkRenderPass m_Renderpass;
        std::vector<VkFramebuffer> m_Framebuffers;

        std::vector<FrameData> m_Frames;

    private:
        Engine() {}
        ~Engine() {}

        void initVulkan(GLFWwindow* window, const char* name);
        void initSwapchain();
        void initRenderpass();
        void initFramebuffers();
        void initCommands();
        void initSyncStructures();
        void initPipelines();
    };
}
