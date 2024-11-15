#include "Engine.hpp"

#include "Eos/Engine/Initializers.hpp"

#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "Eos/Engine/GlobalData.hpp"

namespace Eos
{
    Engine::Engine()
    {
        m_Window = std::make_shared<Window>();
    }

    Engine* Engine::get()
    {
        static Engine engine{};
        return &engine;
    }

    PipelineBuilder Engine::createPipelineBuilder()
    {
        return PipelineBuilder::begin(&m_Device, &m_Renderpass.renderPass).defaultValues();
    }

    ComputePipelineBuilder Engine::createComputePipelineBuilder()
    {
        return ComputePipelineBuilder::begin(&m_Device);
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

            ComputePipelineBuilder::cleanup();
            PipelineBuilder::cleanup();
            ComputeShader::cleanup();

            m_DescriptorAllocator.cleanup();
            m_DescriptorLayoutCache.cleanup();

            vkDeviceWaitIdle(m_Device);

            m_DeletionQueue.flush();

            vkDestroyRenderPass(m_Device, m_Renderpass.renderPass, nullptr);

            for (size_t i = 0; i < m_Framebuffers.size(); i++)
            {
                vkDestroyFramebuffer(m_Device, m_Framebuffers[i], nullptr);
                vkDestroyImageView(m_Device, m_Swapchain.imageViews[i], nullptr);
            }

            vkDestroySwapchainKHR(m_Device, m_Swapchain.swapchain, nullptr);

            vkDestroyDevice(m_Device, nullptr);
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
            vkDestroyInstance(m_Instance, nullptr);
        }
    }

    void Engine::init(const EngineSetupDetails& details)
    {
        m_SetupDetails = details;

        m_Window->init();

        m_Frames.resize(m_SetupDetails.framesInFlight);

        int width, height;
        glfwGetFramebufferSize(m_Window->getWindow(), &width, &height);

        initVulkan();

        initSwapchain();

        GlobalData::s_Device = &m_Device;
        GlobalData::s_Allocator = &m_Allocator;
        GlobalData::s_DeletionQueue = &m_DeletionQueue;

        if (m_SetupDetails.renderpassCreationFunc.has_value())
            (m_SetupDetails.renderpassCreationFunc.value())(m_Renderpass);
        else
            initDefaultRenderpass();

        initFramebuffers();

        initCommands();
        initSyncStructures();
        initDescriptorSets();

        GraphicsSubmit::setup(&m_GraphicsQueue);
        TransferSubmit::setup(&m_TransferQueue);
        ComputeShader::setup(&m_ComputeQueue);

        initImgui();

        m_Initialized = true;

        EOS_CORE_LOG_INFO("Initialized Engine");
    }

    RenderInformation Engine::preRender()
    {
        static uint32_t currentFrame = 0;
        FrameData& frame = m_Frames[currentFrame % m_SetupDetails.framesInFlight];

        EOS_VK_CHECK(vkWaitForFences(m_Device, 1, &frame.renderFence, true, 1000000000));
        EOS_VK_CHECK(vkResetFences(m_Device, 1, &frame.renderFence));

        uint32_t swapchainImageIndex;

        // Attempt a couple times
        for (int i = 0; i < 2; i++)
        {
            VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain.swapchain,
                    1000000000, frame.presentSemaphore, nullptr, &swapchainImageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                recreateSwapchain();
                continue;
            }
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            {
                EOS_CORE_LOG_ERROR("Vulkan Error: {}", result);
            }

            break;
        }

        EOS_VK_CHECK(vkResetCommandBuffer(frame.commandBuffer, 0));

        VkCommandBuffer cmd = frame.commandBuffer;

        VkCommandBufferBeginInfo cmdBeginInfo{};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;
        cmdBeginInfo.pInheritanceInfo = nullptr;
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        EOS_VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        std::vector<VkClearValue> clearValues;

        if (m_SetupDetails.renderClearValues.has_value())
        {
            clearValues = (m_SetupDetails.renderClearValues.value())();
        }
        else
        {
            VkClearValue background = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
            clearValues = { background };
        }

        VkRenderPassBeginInfo rpInfo{};
        rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpInfo.pNext = nullptr;
        rpInfo.renderPass = m_Renderpass.renderPass;
        rpInfo.renderArea.offset.x = 0;
        rpInfo.renderArea.offset.y = 0;
        rpInfo.renderArea.extent = m_Swapchain.extent;
        rpInfo.framebuffer = m_Framebuffers[swapchainImageIndex];
        rpInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        rpInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

        RenderInformation information;
        information.frame = &frame;
        information.swapchainImageIndex = swapchainImageIndex;
        information.cmd = &cmd;

        currentFrame++;
        if (currentFrame >= m_SetupDetails.framesInFlight)
            currentFrame = 0;

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        return information;
    }

    void Engine::postRender(RenderInformation& information)
    {
        VkCommandBuffer cmd = information.frame->commandBuffer;

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

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
        VkResult result = vkQueuePresentKHR(m_GraphicsQueue.queue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            recreateSwapchain();
        }
        else if (result != VK_SUCCESS)
        {
            EOS_CORE_LOG_ERROR("Vulkan Error: {}", result);
        }
    }

    void Engine::initVulkan()
    {
        vkb::InstanceBuilder builder;
        auto instanceReturn = builder.set_app_name(m_SetupDetails.name)
            .request_validation_layers(true)
            .require_api_version(1, 3, 0)
            .use_default_debug_messenger()
            .build();

        vkb::Instance vkbInstance = instanceReturn.value();

        m_Instance = vkbInstance.instance;
        m_DebugMessenger = vkbInstance.debug_messenger;

        m_Window->createSurface(m_Instance, &m_Surface);

        VkPhysicalDeviceFeatures deviceFeatures{};
        if (m_SetupDetails.float64)
            deviceFeatures.shaderFloat64 = true;

        vkb::PhysicalDeviceSelector selector{ vkbInstance };
        vkb::PhysicalDevice vkbPhysicalDevice = selector.set_minimum_version(1, 3)
            .set_surface(m_Surface)
            .set_required_features(deviceFeatures)
            .select()
            .value();

        vkb::DeviceBuilder vkbDeviceBuilder{ vkbPhysicalDevice };
        vkb::Device vkbDevice = vkbDeviceBuilder.build().value();

        m_Device = vkbDevice.device;
        m_PhysicalDevice = vkbPhysicalDevice.physical_device;

        m_GraphicsQueue.queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_GraphicsQueue.family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

        auto transferQueueRet = vkbDevice.get_queue(vkb::QueueType::transfer);
        if (transferQueueRet) // Transfer Queue
        {
            m_TransferQueue.queue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
            m_TransferQueue.family = vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        }
        else // No transfer queue so just use the graphics queue
        {
            m_TransferQueue = m_GraphicsQueue;
        }

        auto computeQueueRet = vkbDevice.get_queue(vkb::QueueType::compute);
        if (computeQueueRet)
        {
            m_ComputeQueue.queue = vkbDevice.get_queue(vkb::QueueType::compute).value();
            m_ComputeQueue.family = vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        }
        else // If not Compute Queue then use the graphics queue
        {
            m_ComputeQueue = m_GraphicsQueue;
        }

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = m_Instance;
        vmaCreateAllocator(&allocatorInfo, &m_Allocator);

        m_DeletionQueue.pushFunction([=]() {
                vmaDestroyAllocator(m_Allocator);
            });

        EOS_CORE_LOG_INFO("Initialized Vulkan");
    }

    void Engine::initSwapchain()
    {
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        if (m_SetupDetails.vsync) presentMode = VK_PRESENT_MODE_FIFO_KHR;

        VkExtent2D windowExtent = m_Window->getExtent();

        vkb::SwapchainBuilder swapchainBuilder {m_PhysicalDevice, m_Device, m_Surface};
        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_format(m_SetupDetails.swapchainFormat)
            .set_desired_present_mode(presentMode)
            .set_desired_extent(windowExtent.width, windowExtent.height)
            .build()
            .value();

        m_Swapchain.swapchain = vkbSwapchain.swapchain;
        m_Swapchain.imageFormat = vkbSwapchain.image_format;
        m_Swapchain.extent = windowExtent;
        m_Swapchain.images = vkbSwapchain.get_images().value();
        m_Swapchain.imageViews = vkbSwapchain.get_image_views().value();

        EOS_CORE_LOG_INFO("Created Swapchain");
    }

    void Engine::initDefaultRenderpass()
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

        VkSubpassDependency colourDependency{};
        colourDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        colourDependency.dstSubpass = 0;
        colourDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        colourDependency.srcAccessMask = 0;
        colourDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        colourDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        RenderPassBuilder::begin(m_Renderpass)
            .addAttachment(colourAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    colourDependency)
            .build();

        EOS_CORE_LOG_INFO("Created Default Renderpass");
    }

    void Engine::initFramebuffers()
    {
        VkExtent2D windowExtent = m_Window->getExtent();

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = nullptr;
        framebufferInfo.renderPass = m_Renderpass.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.width = windowExtent.width;
        framebufferInfo.height = windowExtent.height;
        framebufferInfo.layers = 1;

        const size_t swapchainImageCount = m_Swapchain.images.size();
        m_Framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);

        for (size_t i = 0; i < swapchainImageCount; i++)
        {
            std::vector<VkImageView> attachments;
            if (m_SetupDetails.framebufferCreationFunc.has_value())
            {
                attachments = (m_SetupDetails.framebufferCreationFunc.value())(
                        framebufferInfo, m_Swapchain.imageViews[i], m_Renderpass);
            }
            else
            {
                attachments = { m_Swapchain.imageViews[i] };
            }

            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();

            EOS_VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr,
                        &m_Framebuffers[i]));
        }

        EOS_CORE_LOG_INFO("Created Framebuffer");
    }

    void Engine::initCommands()
    {
        VkCommandPoolCreateInfo commandPoolInfo = Init::commandPoolCreateInfo(
                m_GraphicsQueue.family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        for (uint8_t i = 0; i < m_SetupDetails.framesInFlight; i++)
        {
            FrameData& frame = m_Frames[i];
            EOS_VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo,
                        nullptr, &frame.commandPool));

            VkCommandBufferAllocateInfo cmdAllocInfo = Init::commandBufferAllocateInfo(
                    frame.commandPool, 1);

            EOS_VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo,
                        &frame.commandBuffer));

            m_DeletionQueue.pushFunction([=]()
                    { vkDestroyCommandPool(m_Device, m_Frames[i].commandPool,
                            nullptr); });
        }

        EOS_CORE_LOG_INFO("Created Commands Pools");
    }

    void Engine::initSyncStructures()
    {
        VkFenceCreateInfo fenceCreateInfo = Init::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        VkSemaphoreCreateInfo semaphoreCreateInfo = Init::semaphoreCreateInfo();

        for (uint32_t i = 0; i < m_SetupDetails.framesInFlight; i++)
        {
            FrameData& frame = m_Frames[i];
            EOS_VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr,
                        &frame.renderFence));

            EOS_VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr,
                        &frame.presentSemaphore));
            EOS_VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr,
                        &frame.renderSemaphore));

            m_DeletionQueue.pushFunction([=]() {
                    vkDestroyFence(m_Device, m_Frames[i].renderFence, nullptr);
                    vkDestroySemaphore(m_Device, m_Frames[i].presentSemaphore, nullptr);
                    vkDestroySemaphore(m_Device, m_Frames[i].renderSemaphore, nullptr);
                });
        }

        EOS_CORE_LOG_INFO("Created Sync Structures");
    }

    void Engine::initDescriptorSets()
    {
        m_DescriptorLayoutCache.init(m_Device);
        m_DescriptorAllocator.init(m_Device);

        EOS_CORE_LOG_INFO("Created Descriptor sets");
    }

    void Engine::initImgui()
    {
        VkDescriptorPoolSize poolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo poolCI{};
        poolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCI.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolCI.maxSets = 1000;
        poolCI.poolSizeCount = std::size(poolSizes);
        poolCI.pPoolSizes = poolSizes;

        VkDescriptorPool imguiPool;
        EOS_VK_CHECK(vkCreateDescriptorPool(m_Device, &poolCI, nullptr, &imguiPool));

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(m_Window->getWindow(), true);

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = m_Instance;
        initInfo.PhysicalDevice = m_PhysicalDevice;
        initInfo.Device = m_Device;
        initInfo.Queue = m_GraphicsQueue.queue;
        initInfo.DescriptorPool = imguiPool;
        initInfo.MinImageCount = 3;
        initInfo.ImageCount = 3;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&initInfo, m_Renderpass.renderPass);

        Eos::GraphicsSubmit::submit([&](VkCommandBuffer cmd)
        {
            ImGui_ImplVulkan_CreateFontsTexture(cmd);
        });

        ImGui_ImplVulkan_DestroyFontUploadObjects();

        m_DeletionQueue.pushFunction([=]()
        {
            vkDestroyDescriptorPool(m_Device, imguiPool, nullptr);
            ImGui_ImplVulkan_Shutdown();
        });

        GlobalData::s_ImguiContext = ImGui::GetCurrentContext();
    }

    void Engine::recreateSwapchain()
    {
        EOS_CORE_LOG_INFO("Recreating Swapchain, Framebuffer, and Renderpass");

        vkDeviceWaitIdle(m_Device);

        vkDestroyRenderPass(m_Device, m_Renderpass.renderPass, nullptr);

        for (size_t i = 0; i < m_Framebuffers.size(); i++)
        {
            vkDestroyFramebuffer(m_Device, m_Framebuffers[i], nullptr);
            vkDestroyImageView(m_Device, m_Swapchain.imageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(m_Device, m_Swapchain.swapchain, nullptr);

        EOS_DISABLE_LOGGER();

        initSwapchain();

        if (m_SetupDetails.renderpassCreationFunc.has_value())
            (m_SetupDetails.renderpassCreationFunc.value())(m_Renderpass);
        else
            initDefaultRenderpass();

        initFramebuffers();

        EOS_ENABLE_LOGGER();
    }
}
