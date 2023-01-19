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

    VkViewport Window::getViewport() const
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_WindowSize.x);
        viewport.height = static_cast<float>(m_WindowSize.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        return viewport;
    }

    VkRect2D Window::getScissor() const
    {
        VkRect2D scissor{};

        scissor.offset = { 0, 0 };
        scissor.extent = getWindowExtent();

        return scissor;
    }

    void Window::glfwErrorCallback(int, const char* errStr)
    {
        EOS_LOG_ERROR("{}", errStr);
    }
}
