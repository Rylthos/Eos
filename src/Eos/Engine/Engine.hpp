#pragma once

#include "../Core/Core.hpp"

#include "../Util/DeletionQueue.hpp"

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

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkSurfaceKHR m_Surface;

        VmaAllocator m_Allocator;

        VkQueue m_GraphicsQueue;
        uint32_t m_GraphicsQueueFamily;

    private:
        Engine() {}
        ~Engine() {}

        void initVulkan(GLFWwindow* window, const char* name);
    };
}
