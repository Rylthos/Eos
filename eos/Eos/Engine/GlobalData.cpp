#include "GlobalData.hpp"

namespace Eos
{
    VkDevice* GlobalData::s_Device;
    VmaAllocator* GlobalData::s_Allocator;
    DeletionQueue* GlobalData::s_DeletionQueue;

    ImGuiContext* GlobalData::s_ImguiContext;
}
