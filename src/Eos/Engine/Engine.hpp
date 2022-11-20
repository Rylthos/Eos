#pragma once

#include "../Core/Core.hpp"

#include "../Util/DeletionQueue.hpp"
#include "../Util/Types.hpp"

#include <cstdint>

#include <GLFW/glfw3.h>
#include <Vulkan/Vulkan.h>
#include <vk_mem_alloc.h>

namespace Eos
{
    class EOS_API Engine
    {
    public:
        static Engine& get();
        DeletionQueue& getDeletionQueue();

        void cleanup();
        void init(GLFWwindow* window, const char* name);

    private:
        bool m_Initialized = false;
        VkExtent2D m_WindowExtent;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkSurfaceKHR m_Surface;

        VmaAllocator m_Allocator;

        Queue m_GraphicsQueue;
        Swapchain m_Swapchain;

    private:
        Engine() {}
        ~Engine() {}

        void initVulkan(GLFWwindow* window, const char* name);
        void initSwapchain();
    };
}
