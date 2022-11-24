#include "Application.hpp"
#include "../Engine/Engine.hpp"

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
        m_Engine = Engine::get();

        m_Window = init();

        m_Engine->init(m_Window, m_Details.name.c_str());

        m_Engine->getPipelineBuilder()->defaultPipelineValues();

        postInit();

        mainLoop();
    }

    void Application::mainLoop()
    {
        while(!glfwWindowShouldClose(m_Window))
        {
            glfwPollEvents();

            update(0.0f);

            RenderInformation info = m_Engine->preRender(1);

            draw(*(info.cmd));

            m_Engine->postRender(info);
        }
    }

    GLFWwindow* Application::init() { return nullptr; }
    void Application::postInit() {}
    void Application::draw(VkCommandBuffer cmd) {}
    void Application::update(float dt) {}
}
