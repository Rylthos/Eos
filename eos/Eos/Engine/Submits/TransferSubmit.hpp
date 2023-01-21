#pragma once

#include "Eos/EosPCH.hpp"

#include <vulkan/vulkan.h>

namespace Eos
{
    class EOS_API TransferSubmit
    {
    public:
        static void setup(Queue* queue);
        
        static void submit(std::function<void(VkCommandBuffer)>&& function);
    private:
        static UploadContext s_UploadContext;
        static Queue* s_TransferQueue;
    private:
        TransferSubmit() {}
        ~TransferSubmit() {}
    };
}
