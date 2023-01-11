#pragma once

#include "Eos/EosPCH.hpp"
#include "Eos/Core/Window.hpp"

#include "Eos/Util/DeletionQueue.hpp"

#include "Eos/Engine/Mesh.hpp"
#include "Eos/Engine/PipelineBuilder.hpp"
#include "Eos/Engine/Shader.hpp"

#include "Eos/Engine/DescriptorSets/DescriptorAllocator.hpp"
#include "Eos/Engine/DescriptorSets/DescriptorLayoutCache.hpp"
#include "Eos/Engine/DescriptorSets/DescriptorBuilder.hpp"

#include <Vulkan/Vulkan.h>
#include <vk_mem_alloc.h>

#define EOS_VK_CHECK(x) \
    do \
    { \
        VkResult err = x; \
        if (err) \
        { \
            EOS_LOG_ERROR("Vulkan Error: {}", err); \
            abort(); \
        } \
    } while (0) \

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
        DeletionQueue* getDeletionQueue();
        PipelineBuilder* getPipelineBuilder();
        VkDevice* getDevice();
        VmaAllocator* getAllocator();
        DescriptorBuilder createDescriptorBuilder();

        void cleanup();
        void init(Window& window, const EngineSetupDetails& setupDetails);

        VkPipelineLayoutCreateInfo createPipelineLayoutCreateInfo();
        VkPipelineLayout setupPipelineLayout();
        VkPipelineLayout setupPipelineLayout(VkPipelineLayoutCreateInfo info);
        VkPipeline setupPipeline(VkPipelineLayout layout);

        RenderInformation preRender(int frameNumber);
        void postRender(RenderInformation& information);

        Buffer createBuffer(size_t allocSize, VkBufferUsageFlags usage,
                VmaMemoryUsage memoryUsage);

        void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

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
        Swapchain m_Swapchain;

        VkRenderPass m_Renderpass;
        std::vector<VkFramebuffer> m_Framebuffers;

        std::vector<FrameData> m_Frames;

        UploadContext m_UploadContext;

        DescriptorAllocator m_DescriptorAllocator;
        DescriptorLayoutCache m_DescriptorLayoutCache;

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
        void initUploadContext();

    public:
        template <VertexTemplate T>
        void createMesh(Mesh<T>& mesh)
        {
            const size_t bufferSize = mesh.getVertices()->size() * mesh.getVertexSize();

            Buffer stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VMA_MEMORY_USAGE_CPU_ONLY);

            void* data;
            vmaMapMemory(m_Allocator, stagingBuffer.allocation, &data);
                memcpy(data, mesh.getVertices()->data(), bufferSize);
            vmaUnmapMemory(m_Allocator, stagingBuffer.allocation);

            mesh.setVertexBuffer(createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY));

            immediateSubmit([=](VkCommandBuffer cmd) {
                    VkBufferCopy copy;
                    copy.srcOffset = 0;
                    copy.dstOffset = 0;
                    copy.size = bufferSize;
                    vkCmdCopyBuffer(cmd, stagingBuffer.buffer,
                            mesh.getVertexBuffer()->buffer, 1, &copy);
                    });

            getDeletionQueue()->pushFunction([=]() {
                    vmaDestroyBuffer(m_Allocator, mesh.getVertexBuffer()->buffer,
                            mesh.getVertexBuffer()->allocation);
                    });
            vmaDestroyBuffer(m_Allocator, stagingBuffer.buffer,
                    stagingBuffer.allocation);
        }

        template <VertexTemplate T>
        void updateMesh(Mesh<T>& mesh) { createMesh(mesh); }

        template <VertexTemplate T, typename I>
            requires std::is_integral<I>::value
        void createIndexedMesh(IndexedMesh<T, I>& mesh)
        {
            createMesh(mesh);

            const size_t bufferSize = mesh.getIndices()->size() * mesh.getIndexSize();

            Buffer stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VMA_MEMORY_USAGE_CPU_ONLY);

            void* data;
            vmaMapMemory(m_Allocator, stagingBuffer.allocation, &data);
                memcpy(data, mesh.getIndices()->data(), bufferSize);
            vmaUnmapMemory(m_Allocator, stagingBuffer.allocation);

            mesh.setIndexBuffer(createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY));

            immediateSubmit([=](VkCommandBuffer cmd) {
                    VkBufferCopy copy;
                    copy.srcOffset = 0;
                    copy.dstOffset = 0;
                    copy.size = bufferSize;
                    vkCmdCopyBuffer(cmd, stagingBuffer.buffer,
                            mesh.getIndexBuffer()->buffer, 1, &copy);
                    });

            getDeletionQueue()->pushFunction([=]() {
                    vmaDestroyBuffer(m_Allocator, mesh.getIndexBuffer()->buffer,
                            mesh.getIndexBuffer()->allocation);
                    });
            vmaDestroyBuffer(m_Allocator, stagingBuffer.buffer,
                    stagingBuffer.allocation);
        }

        template <VertexTemplate T, typename I>
            requires std::is_integral<I>::value
        void updateIndexedMesh(IndexedMesh<T, I>& mesh) { createIndexedMesh(mesh); }
    };
}
