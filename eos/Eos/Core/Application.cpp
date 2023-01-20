#include "Application.hpp"

#define VMA_IMPLEMENTATION
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
        PipelineBuilder::cleanup();

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
            m_Details.enableVsync
        };

        if (m_Details.customRenderpass)
        {
            engineSetupDetails.renderpassCreationFunc = std::make_optional(
                    std::bind(&Application::renderPassInit, this,
                        std::placeholders::_1));
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
        int currentFrame = 0;

        while(!m_Window.shouldClose())
        {
            glfwPollEvents();

            m_FrameTimer.tick();
            update(m_FrameTimer.timeElapsed());

            RenderInformation info = m_Engine->preRender(currentFrame);

            draw(*(info.cmd));

            m_Engine->postRender(info);
            currentFrame++;
        }
        m_FrameTimer.end();
    }

    void Application::windowInit() {}
    void Application::renderPassInit(RenderPass& renderPass)
        { EOS_LOG_CRITICAL("This needs to be overriden when customRenderpass is defined"); }
    void Application::postEngineInit() {}
    void Application::draw(VkCommandBuffer cmd) {}
    void Application::update(double dt) {}
}
