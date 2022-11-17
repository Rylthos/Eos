#include "Application.hpp"

namespace Eos
{
    Application::Application()
    {

    }

    Application::~Application()
    {

    }

    void Application::start()
    {
        m_Window = init();

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
    void Application::draw() {}
    void Application::update(float dt) {}
}
