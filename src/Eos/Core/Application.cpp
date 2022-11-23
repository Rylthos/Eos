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
        Engine::get().cleanup();
    }

    void Application::start()
    {
        m_Window = init();

        Engine::get().init(m_Window, m_Details.name.c_str());

        Engine::get().getPipelineBuilder().defaultPipelineValues();

        postInit();

        mainLoop();
    }

    void Application::mainLoop()
    {
        while(!glfwWindowShouldClose(m_Window))
        {
            glfwPollEvents();

            update(0.0f);
            draw();
        }
    }

    GLFWwindow* Application::init() { return nullptr; }
    void Application::postInit() {}
    void Application::draw() {}
    void Application::update(float dt) {}
}
