#pragma once

#include "../Core/Core.hpp"
#include "../Core/Window.hpp"

#include "../Util/DeletionQueue.hpp"
#include "../Util/Types.hpp"

#include "Mesh.hpp"
#include "PipelineBuilder.hpp"
#include "Shader.hpp"

#include <cstdint>

#include <Vulkan/Vulkan.h>
#include <vk_mem_alloc.h>

#include <iostream>

#define EOS_VK_CHECK(x) \
    do \
    { \
        VkResult err = x; \
        if (err) \
        { \
            std::cout << "Detected Vulkan Error: " << err << "\n"; \
            abort(); \
        } \
    } while (0) \

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

        void cleanup();
        void init(Window& window, const char* name);

        VkPipelineLayout setupPipelineLayout();
        VkPipeline setupPipeline(VkPipelineLayout layout);

        RenderInformation preRender(int frameNumber);
        void postRender(RenderInformation& information);

        void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

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

        UploadContext m_UploadContext;

    private:
        Engine() {}
        ~Engine() {}

        void initVulkan(Window& window, const char* name);
        void initSwapchain();
        void initRenderpass();
        void initFramebuffers();
        void initCommands();
        void initSyncStructures();
        void initUploadContext();

    public:
        template <VertexDescription T>
        void createMesh(Mesh<T>& mesh)
        {
            const size_t bufferSize = mesh.getVertices()->size() * mesh.getVertexSize();
            VkBufferCreateInfo stagingBufferInfo{};
            stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            stagingBufferInfo.pNext = nullptr;
            stagingBufferInfo.size = bufferSize;
            stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo vmaAllocInfo{};
            vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

            Buffer stagingBuffer;
            EOS_VK_CHECK(vmaCreateBuffer(m_Allocator, &stagingBufferInfo,
                        &vmaAllocInfo, &stagingBuffer.buffer, &stagingBuffer.allocation,
                        nullptr));

            void* data;
            vmaMapMemory(m_Allocator, stagingBuffer.allocation, &data);
                memcpy(data, mesh.getVertices()->data(), bufferSize);
            vmaUnmapMemory(m_Allocator, stagingBuffer.allocation);

            VkBufferCreateInfo vertexBufferInfo{};
            vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            vertexBufferInfo.pNext = nullptr;
            vertexBufferInfo.size = bufferSize;
            vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT;

            vmaAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            EOS_VK_CHECK(vmaCreateBuffer(m_Allocator, &vertexBufferInfo,
                        &vmaAllocInfo, &mesh.getVertexBuffer()->buffer,
                        &mesh.getVertexBuffer()->allocation, nullptr));

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

        template <VertexDescription T>
        void createIndexedMesh(IndexedMesh<T>& mesh)
        {
            createMesh(mesh);

            const size_t bufferSize = mesh.getIndices()->size() * mesh.getIndexSize();
            VkBufferCreateInfo stagingBufferInfo{};
            stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            stagingBufferInfo.pNext = nullptr;
            stagingBufferInfo.size = bufferSize;
            stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo vmaAllocInfo{};
            vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

            Buffer stagingBuffer;
            EOS_VK_CHECK(vmaCreateBuffer(m_Allocator, &stagingBufferInfo,
                        &vmaAllocInfo, &stagingBuffer.buffer, &stagingBuffer.allocation,
                        nullptr));

            void* data;
            vmaMapMemory(m_Allocator, stagingBuffer.allocation, &data);
                memcpy(data, mesh.getIndices()->data(), bufferSize);
            vmaUnmapMemory(m_Allocator, stagingBuffer.allocation);

            VkBufferCreateInfo vertexBufferInfo{};
            vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            vertexBufferInfo.pNext = nullptr;
            vertexBufferInfo.size = bufferSize;
            vertexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT;

            vmaAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            EOS_VK_CHECK(vmaCreateBuffer(m_Allocator, &vertexBufferInfo,
                        &vmaAllocInfo, &mesh.getIndexBuffer()->buffer,
                        &mesh.getIndexBuffer()->allocation, nullptr));

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
    };
}
