#include "Application.hpp"
#include "../Engine/Engine.hpp"

#include <stdexcept>

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
        init();

        m_Engine = Engine::get();

        if (!m_Window.isValid())
        {
            std::runtime_error("Window has not been initialised");
        }

        m_Engine->init(m_Window, m_Details.name.c_str());
        m_Engine->getPipelineBuilder()->defaultPipelineValues();

        postInit();

        mainLoop();
    }

    void Application::mainLoop()
    {
        while(!m_Window.shouldClose())
        {
            glfwPollEvents();

            update(0.0f);

            RenderInformation info = m_Engine->preRender(1);

            draw(*(info.cmd));

            m_Engine->postRender(info);
        }
    }

    void Application::init() {}
    void Application::postInit() {}
    void Application::draw(VkCommandBuffer cmd) {}
    void Application::update(float dt) {}
}
