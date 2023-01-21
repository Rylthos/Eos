#include "Application.hpp"

#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "Eos/Engine/Engine.hpp"

#include <stdexcept>

#include <functional>

namespace Eos
{
    Application::Application(const ApplicationDetails& details)
    {
        m_Details = details;
    }

    Application::~Application()
    {
        m_Engine->cleanup();
    }

    void Application::start()
    {
        m_Window.init();

        m_Window.setWindowHint(GLFW_RESIZABLE,
                m_Details.enableWindowResizing ? GLFW_TRUE : GLFW_FALSE);

        windowInit();
        m_MainEventListener.addListeners(m_Window, &m_MainEventDispatcher);

        EOS_LOG_INFO("Initialised Application");

        m_Engine = Engine::get();

        if (!m_Window.isValid())
        {
            std::runtime_error("Window has not been initialised");
        }

        EngineSetupDetails engineSetupDetails = {
            m_Details.name.c_str(),
            m_Details.enableVsync,
            m_Details.framesInFlight
        };

        if (m_Details.customRenderpass)
        {
            engineSetupDetails.renderpassCreationFunc = std::make_optional(
                    std::bind(&Application::renderPassInit, this,
                        std::placeholders::_1));

            engineSetupDetails.framebufferCreationFunc = std::make_optional(
                    std::bind(&Application::framebufferCreation, this,
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3));
        }

        if (m_Details.customClearValues)
        {
            engineSetupDetails.renderClearValues = std::make_optional(
                    std::bind(&Application::renderClearValues, this));
        }

        m_Engine->init(m_Window, engineSetupDetails);

        postEngineInit();

        mainLoop();

        // For Destructors
        vkDeviceWaitIdle(GlobalData::getDevice());
    }

    void Application::mainLoop()
    {
        m_FrameTimer.start();

        while(!m_Window.shouldClose())
        {
            glfwPollEvents();

            m_FrameTimer.tick();
            update(m_FrameTimer.timeElapsed());

            RenderInformation info = m_Engine->preRender();

            draw(*(info.cmd));

            m_Engine->postRender(info);
        }
        m_FrameTimer.end();
    }

    void Application::windowInit() {}

    void Application::renderPassInit(RenderPass& renderPass)
    { EOS_LOG_CRITICAL("(renderPassInit) This needs to be overriden when customRenderpass is set"); }

    std::vector<VkImageView> Application::framebufferCreation(VkFramebufferCreateInfo& framebuffer,
            VkImageView& swapchainImage, RenderPass& renderpass)
    { EOS_LOG_CRITICAL("(framebufferCreation) This needs to be overriden when customRenderpass is set");
        return {}; };

    std::vector<VkClearValue> Application::renderClearValues()
    { EOS_LOG_CRITICAL("(renderClearValues) This needs to be overriden when customClearValues is set");
        return {}; };

    void Application::postEngineInit() {}
    void Application::draw(VkCommandBuffer cmd) {}
    void Application::update(double dt) {}
}
