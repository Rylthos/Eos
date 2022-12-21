#include "Window.hpp"

#include "Eos/Core/Logger.hpp"

#include <iostream>

namespace Eos
{
    Window::Window()
        : m_WindowSize({ 0, 0 })
    { }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::init()
    {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
        {
            EOS_LOG_CRITICAL("Failed to init GLFW");
            exit(-1);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Initialised = true;

        EOS_LOG_INFO("Initialised GLFW");
    }

    Window& Window::create(const char* title)
    {
        m_Window = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y,
                title, nullptr, nullptr);

        m_Created = true;

        EOS_LOG_INFO("Created Window \"{}\" - Size {}:{}", title,
                m_WindowSize.x, m_WindowSize.y);

        return *this;
    }

    void Window::createSurface(VkInstance& instance, VkSurfaceKHR* surface) const
    {
        glfwCreateWindowSurface(instance, m_Window, nullptr, surface);
    }

    void Window::glfwErrorCallback(int, const char* errStr)
    {
        EOS_LOG_ERROR("{}", errStr);
    }
}
