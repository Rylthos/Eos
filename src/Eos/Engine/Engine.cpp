#include "Engine.hpp"

#include "../Util/Initializers.hpp"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
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
    Engine& Engine::get()
    {
        static Engine engine{};
        return engine;
    }

    DeletionQueue& Engine::getDeletionQueue()
    {
        static DeletionQueue queue{};
        return queue;
    }

    PipelineBuilder& Engine::getPipelineBuilder()
    {
        static PipelineBuilder builder{};
        return builder;
    }

    VkDevice& Engine::getDevice()
    {
        return m_Device;
    }

    void Engine::cleanup()
    {
        if (m_Initialized)
        {
            getDeletionQueue().flush();

            vkDestroyDevice(m_Device, nullptr);
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
            vkDestroyInstance(m_Instance, nullptr);
        }
    }

    void Engine::init(GLFWwindow* window, const char* name)
    {
        m_Frames.resize(m_FrameOverlap);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        m_WindowExtent = VkExtent2D{
            static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        initVulkan(window, name);

        initSwapchain();
        initRenderpass();
        initFramebuffers();
        initCommands();
        initSyncStructures();

        m_Initialized = true;
    }

    VkPipelineLayout Engine::setupPipelineLayout()
    {
        VkPipelineLayout layout;
        VkPipelineLayoutCreateInfo info = Pipeline::pipelineLayoutCreateInfo();

        EOS_VK_CHECK(vkCreatePipelineLayout(m_Device, &info, nullptr, &layout));

        getDeletionQueue().pushFunction([=]() {
                vkDestroyPipelineLayout(m_Device, layout, nullptr); 
            });

        return layout;
    }

    VkPipeline Engine::setupPipeline(VkPipelineLayout layout)
    {
        getPipelineBuilder().pipelineLayout = layout;
        VkPipeline pipeline = getPipelineBuilder().buildPipeline(m_Device, m_Renderpass);

        getDeletionQueue().pushFunction([=]() {
                vkDestroyPipeline(m_Device, pipeline, nullptr); 
            });

        return pipeline;
    }

    void Engine::initVulkan(GLFWwindow* window, const char* name)
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

        glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface);

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

        getDeletionQueue().pushFunction([=]() {
                vmaDestroyAllocator(m_Allocator);
            });
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

        getDeletionQueue().pushFunction([=]()
                { vkDestroySwapchainKHR(m_Device, m_Swapchain.swapchain, nullptr); });
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

        getDeletionQueue().pushFunction([&]()
                { vkDestroyRenderPass(m_Device, m_Renderpass, nullptr); });
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

            getDeletionQueue().pushFunction([=]() {
                    vkDestroyFramebuffer(m_Device, m_Framebuffers[i], nullptr);
                    vkDestroyImageView(m_Device, m_Swapchain.imageViews[i], nullptr);
                });
        }
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

            getDeletionQueue().pushFunction([=]()
                    { vkDestroyCommandPool(m_Device, m_Frames[i].commandPool,
                            nullptr); });
        }
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

            getDeletionQueue().pushFunction([=]() {
                    vkDestroyFence(m_Device, m_Frames[i].renderFence, nullptr);
                    vkDestroySemaphore(m_Device, m_Frames[i].presentSemaphore, nullptr);
                    vkDestroySemaphore(m_Device, m_Frames[i].renderSemaphore, nullptr);
                });
        }
    }
}

#undef EOS_VK_CHECK
