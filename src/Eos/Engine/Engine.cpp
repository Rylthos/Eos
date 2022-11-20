#include "Engine.hpp"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <iostream>

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
        initVulkan(window, name);

        m_Initialized = true;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        m_WindowExtent = VkExtent2D{
            static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
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
}
