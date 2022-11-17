#include "Engine.hpp"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <iostream>

namespace Eos
{
    void Engine::cleanup()
    {
        if (m_Initialized)
        {
            vmaDestroyAllocator(m_Allocator);

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

        m_GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = m_Instance;
        vmaCreateAllocator(&allocatorInfo, &m_Allocator);
    }

    bool Engine::m_Initialized = false;
    VkInstance Engine::m_Instance;
    VkDebugUtilsMessengerEXT Engine::m_DebugMessenger;
    VkPhysicalDevice Engine::m_PhysicalDevice;
    VkDevice Engine::m_Device;
    VkSurfaceKHR Engine::m_Surface;

    VmaAllocator Engine::m_Allocator;

    VkQueue Engine::m_GraphicsQueue;
    uint32_t Engine::m_GraphicsQueueFamily;
}
