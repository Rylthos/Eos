#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Core/DeletionQueue.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Eos
{
    class EOS_API GlobalData
    {
    public:
        static VkDevice& getDevice() { return *s_Device; }

        static VmaAllocator& getAllocator() { return *s_Allocator; }

        static DeletionQueue& getDeletionQueue() { return *s_DeletionQueue; }
    private:
        friend class Engine;

        static VkDevice* s_Device;
        static VmaAllocator* s_Allocator;

        static DeletionQueue* s_DeletionQueue;
    private:
        GlobalData() {}
        ~GlobalData() {}
    };
}
