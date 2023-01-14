#include "Application.hpp"

#define VMA_IMPLEMENTATION
#include "Eos/Engine/Engine.hpp"

#include <stdexcept>

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

        init();
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

        m_Engine->init(m_Window, engineSetupDetails);

        postInit();

        mainLoop();
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

    void Application::init() {}
    void Application::postInit() {}
    void Application::draw(VkCommandBuffer cmd) {}
    void Application::update(double dt) {}
}
