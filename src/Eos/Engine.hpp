#pragma once

#include "Core.hpp"

#include <cstdint>

#include <GLFW/glfw3.h>
#include <Vulkan/Vulkan.h>
#include <vk_mem_alloc.h>

namespace Eos
{
    class EOS_API Engine
    {
    public:
        static void cleanup();
        static void init(GLFWwindow* window, const char* name);

    private:
        static bool m_Initialized;
        static VkInstance m_Instance;
        static VkDebugUtilsMessengerEXT m_DebugMessenger;
        static VkPhysicalDevice m_PhysicalDevice;
        static VkDevice m_Device;
        static VkSurfaceKHR m_Surface;

        static VmaAllocator m_Allocator;

        static VkQueue m_GraphicsQueue;
        static uint32_t m_GraphicsQueueFamily;

    private:
        Engine() {}
        ~Engine() {}

        static void initVulkan(GLFWwindow* window, const char* name);
    };
}
