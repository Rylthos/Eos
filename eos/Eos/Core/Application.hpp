#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Core/Window.hpp"
#include "Eos/Core/Logger.hpp"
#include "Eos/Core/Timer.hpp"

#include "Eos/Engine/Engine.hpp"
#include "Eos/Engine/Mesh.hpp"
#include "Eos/Engine/Types.hpp"

#include <GLFW/glfw3.h>

#include <string>

#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "Eos/Events/EventListener.hpp"
#include "Eos/Events/EventDispatcher.hpp"

namespace Eos
{
    struct ApplicationDetails
    {
        std::string name;
        bool enableVsync = true;
        bool enableWindowResizing = false;
        bool customRenderpass = false;
        bool customClearValues = false;
        uint32_t framesInFlight = 1;
        VkSurfaceFormatKHR swapchainFormat =
            { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    };

    class EOS_API Application
    {
    public:
        Application(const ApplicationDetails& details);
        virtual ~Application();

        void start();

    protected:
        Engine* m_Engine;
        std::shared_ptr<Window>& m_Window;

        Events::EventDispatcher m_MainEventDispatcher;
    private:
        ApplicationDetails m_Details;
        Timer m_FrameTimer;

        Events::EventListener m_MainEventListener;
    private:
        void mainLoop();

        virtual void windowInit();

        virtual void renderPassInit(RenderPass& renderPass);

        virtual std::vector<VkImageView> framebufferCreation(
                VkFramebufferCreateInfo& framebuffer, VkImageView& swapchainImage,
                RenderPass& renderpass);


        virtual void postEngineInit();

        virtual std::vector<VkClearValue> renderClearValues();

        virtual void draw(VkCommandBuffer cmd);
        virtual void update(double dt);
    };

    Application* createApplication();
}
