#include "Engine.hpp"

#include "../Util/Initializers.hpp"

#include <VkBootstrap.h>

#include <vk_mem_alloc.h>

#include <iostream>

#include "../Core/Logger.hpp"

namespace Eos
{
    Engine* Engine::get()
    {
        static Engine engine{};
        return &engine;
    }

    DeletionQueue* Engine::getDeletionQueue()
    {
        static DeletionQueue queue{};
        return &queue;
    }

    PipelineBuilder* Engine::getPipelineBuilder()
    {
        static PipelineBuilder builder{};
        return &builder;
    }

    VmaAllocator* Engine::getAllocator()
    {
        return &m_Allocator;
    }

    VkDevice* Engine::getDevice()
    {
        return &m_Device;
    }

    DescriptorBuilder Engine::createDescriptorBuilder()
    {
        return DescriptorBuilder::begin(&m_DescriptorLayoutCache, &m_DescriptorAllocator);
    }

    void Engine::cleanup()
    {
        if (m_Initialized)
        {
            vkDeviceWaitIdle(m_Device);

            m_DescriptorAllocator.cleanup();
            m_DescriptorLayoutCache.cleanup();

            getDeletionQueue()->flush();

            vkDestroyDevice(m_Device, nullptr);
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
            vkDestroyInstance(m_Instance, nullptr);
        }
    }

    void Engine::init(Window& window, const char* name)
    {
        m_Frames.resize(m_FrameOverlap);

        int width, height;
        glfwGetFramebufferSize(window.getWindow(), &width, &height);
        m_WindowExtent = VkExtent2D{
            static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        initVulkan(window, name);

        initSwapchain();
        initRenderpass();
        initFramebuffers();
        initCommands();
        initSyncStructures();
        initDescriptorSets();
        initUploadContext();

        m_Initialized = true;

        EOS_LOG_INFO("Initialized Engine");
    }

    VkPipelineLayoutCreateInfo Engine::createPipelineLayoutCreateInfo()
    {
        return Pipeline::pipelineLayoutCreateInfo();
    }

    VkPipelineLayout Engine::setupPipelineLayout()
    {
        VkPipelineLayoutCreateInfo info = Pipeline::pipelineLayoutCreateInfo();
        return setupPipelineLayout(info);
    }

    VkPipelineLayout Engine::setupPipelineLayout(VkPipelineLayoutCreateInfo info)
    {
        VkPipelineLayout layout;

        EOS_VK_CHECK(vkCreatePipelineLayout(m_Device, &info, nullptr, &layout));

        getDeletionQueue()->pushFunction([=]() {
                vkDestroyPipelineLayout(m_Device, layout, nullptr); 
            });

        return layout;
    }

    RenderInformation Engine::preRender(int frameNumber)
    {
        FrameData& frame = m_Frames[frameNumber % m_FrameOverlap];

        EOS_VK_CHECK(vkWaitForFences(m_Device, 1, &frame.renderFence, true, 1000000000));
        EOS_VK_CHECK(vkResetFences(m_Device, 1, &frame.renderFence));

        uint32_t swapchainImageIndex;
        EOS_VK_CHECK(vkAcquireNextImageKHR(m_Device, m_Swapchain.swapchain, 1000000000,
                    frame.presentSemaphore, nullptr, &swapchainImageIndex));
        EOS_VK_CHECK(vkResetCommandBuffer(frame.commandBuffer, 0));

        VkCommandBuffer cmd = frame.commandBuffer;

        VkCommandBufferBeginInfo cmdBeginInfo{};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;
        cmdBeginInfo.pInheritanceInfo = nullptr;
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        EOS_VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        VkClearValue background = { { { 0.1f, 0.1f, 0.1f, 1.0f } } };
        VkClearValue clearValues[1] = { background };

        VkRenderPassBeginInfo rpInfo{};
        rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpInfo.pNext = nullptr;
        rpInfo.renderPass = m_Renderpass;
        rpInfo.renderArea.offset.x = 0;
        rpInfo.renderArea.offset.y = 0;
        rpInfo.renderArea.extent = m_WindowExtent;
        rpInfo.framebuffer = m_Framebuffers[swapchainImageIndex];
        rpInfo.clearValueCount = 1;
        rpInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

        RenderInformation information;
        information.frame = &frame;
        information.swapchainImageIndex = swapchainImageIndex;
        information.cmd = &cmd;

        return information;
    }

    void Engine::postRender(RenderInformation& information)
    {
        VkCommandBuffer cmd = information.frame->commandBuffer;
        vkCmdEndRenderPass(cmd);
        EOS_VK_CHECK(vkEndCommandBuffer(cmd));

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pNext = nullptr;
        submit.pWaitDstStageMask = &waitStage;
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &information.frame->presentSemaphore;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &information.frame->renderSemaphore;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;

        EOS_VK_CHECK(vkQueueSubmit(m_GraphicsQueue.queue, 1, &submit, information.frame->renderFence));
        
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain.swapchain;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &information.frame->renderSemaphore;
        presentInfo.pImageIndices = &information.swapchainImageIndex;
        EOS_VK_CHECK(vkQueuePresentKHR(m_GraphicsQueue.queue, &presentInfo));
    }

    Buffer Engine::createBuffer(size_t allocSize, VkBufferUsageFlags usage,
            VmaMemoryUsage memoryUsage)
    {
        VkBufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.size = allocSize;
        info.usage = usage;

        VmaAllocationCreateInfo vmaAllocInfo{};
        vmaAllocInfo.usage = memoryUsage;

        Buffer buffer;
        EOS_VK_CHECK(vmaCreateBuffer(m_Allocator, &info, &vmaAllocInfo,
                    &buffer.buffer, &buffer.allocation, nullptr));

        return buffer;
    }

    void Engine::immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
    {
        VkCommandBuffer cmd = m_UploadContext.commandBuffer;

        VkCommandBufferBeginInfo cmdBeginInfo = Init::commandBufferBeginInfo(
                VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        
        EOS_VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        function(cmd);

        EOS_VK_CHECK(vkEndCommandBuffer(cmd));

        VkSubmitInfo submit = Init::submitInfo(&cmd);

        EOS_VK_CHECK(vkQueueSubmit(m_GraphicsQueue.queue, 1, &submit,
                    m_UploadContext.uploadFence));
        vkWaitForFences(m_Device, 1, &m_UploadContext.uploadFence, true, 9999999999);
        vkResetFences(m_Device, 1, &m_UploadContext.uploadFence);

        vkResetCommandPool(m_Device, m_UploadContext.commandPool, 0);
    }

    VkPipeline Engine::setupPipeline(VkPipelineLayout layout)
    {
        getPipelineBuilder()->pipelineLayout = layout;
        VkPipeline pipeline = getPipelineBuilder()->buildPipeline(m_Device, m_Renderpass);

        getDeletionQueue()->pushFunction([=]() {
                vkDestroyPipeline(m_Device, pipeline, nullptr); 
            });

        return pipeline;
    }

    void Engine::initVulkan(Window& window, const char* name)
    {
        vkb::InstanceBuilder builder;
        auto instanceReturn = builder.set_app_name(name)
            .request_validation_layers(true)
            .require_api_version(1, 3, 0)
            .use_default_debug_messenger()
            .build();

        vkb::Instance vkbInstance = instanceReturn.value();

        m_Instance = vkbInstance.instance;
        m_DebugMessenger = vkbInstance.debug_messenger;

        window.createSurface(m_Instance, &m_Surface);

        vkb::PhysicalDeviceSelector selector{ vkbInstance };
        vkb::PhysicalDevice vkbPhysicalDevice = selector.set_minimum_version(1, 3)
            .set_surface(m_Surface)
            .select()
            .value();

        vkb::DeviceBuilder vkbDeviceBuilder{ vkbPhysicalDevice };
        vkb::Device vkbDevice = vkbDeviceBuilder.build().value();

        m_Device = vkbDevice.device;
        m_PhysicalDevice = vkbPhysicalDevice.physical_device;

        m_GraphicsQueue.queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_GraphicsQueue.family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = m_Instance;
        vmaCreateAllocator(&allocatorInfo, &m_Allocator);

        getDeletionQueue()->pushFunction([=]() {
                vmaDestroyAllocator(m_Allocator);
            });

        EOS_LOG_INFO("Initialized Vulkan");
    }

    void Engine::initSwapchain()
    {
        vkb::SwapchainBuilder swapchainBuilder {m_PhysicalDevice, m_Device, m_Surface};
        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(m_WindowExtent.width, m_WindowExtent.height)
            .build()
            .value();

        m_Swapchain.swapchain = vkbSwapchain.swapchain;
        m_Swapchain.images = vkbSwapchain.get_images().value();
        m_Swapchain.imageViews = vkbSwapchain.get_image_views().value();
        m_Swapchain.imageFormat = vkbSwapchain.image_format;

        getDeletionQueue()->pushFunction([=]()
                { vkDestroySwapchainKHR(m_Device, m_Swapchain.swapchain, nullptr); });

        EOS_LOG_INFO("Created Swapchain");
    }

    void Engine::initRenderpass()
    {
        VkAttachmentDescription colourAttachment{};
        colourAttachment.format = m_Swapchain.imageFormat;
        colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colourAttachmentRef{};
        colourAttachmentRef.attachment = 0;
        colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDependency colourDependency{};
        colourDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        colourDependency.dstSubpass = 0;
        colourDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        colourDependency.srcAccessMask = 0;
        colourDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        colourDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::vector<VkAttachmentDescription> attachments = { colourAttachment };
        std::vector<VkAttachmentReference> references = { colourAttachmentRef };
        std::vector<VkSubpassDependency> dependencies = { colourDependency };

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = static_cast<uint32_t>(references.size());
        subpass.pColorAttachments = references.data();
        subpass.pDepthStencilAttachment = nullptr;

        VkRenderPassCreateInfo renderpassInfo{};
        renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderpassInfo.pAttachments = attachments.data();
        renderpassInfo.subpassCount = 1;
        renderpassInfo.pSubpasses = &subpass;
        renderpassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderpassInfo.pDependencies = dependencies.data();

        EOS_VK_CHECK(vkCreateRenderPass(m_Device, &renderpassInfo, nullptr, &m_Renderpass));

        getDeletionQueue()->pushFunction([&]()
                { vkDestroyRenderPass(m_Device, m_Renderpass, nullptr); });

        EOS_LOG_INFO("Created Renderpass");
    }

    void Engine::initFramebuffers()
    {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = nullptr;
        framebufferInfo.renderPass = m_Renderpass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.width = m_WindowExtent.width;
        framebufferInfo.height = m_WindowExtent.height;
        framebufferInfo.layers = 1;

        const size_t swapchainImageCount = m_Swapchain.images.size();
        m_Framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);

        for (size_t i = 0; i < swapchainImageCount; i++)
        {
            std::vector<VkImageView> attachments = { m_Swapchain.imageViews[i] };

            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();

            EOS_VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr,
                        &m_Framebuffers[i]));

            getDeletionQueue()->pushFunction([=]() {
                    vkDestroyFramebuffer(m_Device, m_Framebuffers[i], nullptr);
                    vkDestroyImageView(m_Device, m_Swapchain.imageViews[i], nullptr);
                });
        }

        EOS_LOG_INFO("Created Framebuffer");
    }

    void Engine::initCommands()
    {
        VkCommandPoolCreateInfo commandPoolInfo = Init::commandPoolCreateInfo(
                m_GraphicsQueue.family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        for (uint8_t i = 0; i < m_FrameOverlap; i++)
        {
            FrameData& frame = m_Frames[i];
            EOS_VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo,
                        nullptr, &frame.commandPool));

            VkCommandBufferAllocateInfo cmdAllocInfo = Init::commandBufferAllocateInfo(
                    frame.commandPool, 1);

            EOS_VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo,
                        &frame.commandBuffer));

            getDeletionQueue()->pushFunction([=]()
                    { vkDestroyCommandPool(m_Device, m_Frames[i].commandPool,
                            nullptr); });
        }

        EOS_LOG_INFO("Created Commands Pools");
    }

    void Engine::initSyncStructures()
    {
        VkFenceCreateInfo fenceCreateInfo = Init::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        VkSemaphoreCreateInfo semaphoreCreateInfo = Init::semaphoreCreateInfo();

        for (uint32_t i = 0; i < m_FrameOverlap; i++)
        {
            FrameData& frame = m_Frames[i];
            EOS_VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr,
                        &frame.renderFence));

            EOS_VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr,
                        &frame.presentSemaphore));
            EOS_VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr,
                        &frame.renderSemaphore));

            getDeletionQueue()->pushFunction([=]() {
                    vkDestroyFence(m_Device, m_Frames[i].renderFence, nullptr);
                    vkDestroySemaphore(m_Device, m_Frames[i].presentSemaphore, nullptr);
                    vkDestroySemaphore(m_Device, m_Frames[i].renderSemaphore, nullptr);
                });
        }

        EOS_LOG_INFO("Created Sync Structures");
    }

    void Engine::initDescriptorSets()
    {
        m_DescriptorLayoutCache.init(m_Device);
        m_DescriptorAllocator.init(m_Device);

        EOS_LOG_INFO("Created Descriptor sets");
    }

    void Engine::initUploadContext()
    {
        // Pool
        VkCommandPoolCreateInfo uploadCommandPoolInfo = Init::commandPoolCreateInfo(
                m_GraphicsQueue.family);
        EOS_VK_CHECK(vkCreateCommandPool(m_Device, &uploadCommandPoolInfo, nullptr,
                    &m_UploadContext.commandPool));

        // Buffer
        VkCommandBufferAllocateInfo cmdAllocInfo = Init::commandBufferAllocateInfo(
                m_UploadContext.commandPool, 1);

        EOS_VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo,
                    &m_UploadContext.commandBuffer));

        // Fence
        VkFenceCreateInfo uploadFenceCreateInfo = Init::fenceCreateInfo();
        EOS_VK_CHECK(vkCreateFence(m_Device, &uploadFenceCreateInfo, nullptr,
                    &m_UploadContext.uploadFence));

        getDeletionQueue()->pushFunction([=]() {
                vkDestroyCommandPool(m_Device, m_UploadContext.commandPool, nullptr);
                vkDestroyFence(m_Device, m_UploadContext.uploadFence, nullptr);
            });

        EOS_LOG_INFO("Created Upload Context");
    }
}
