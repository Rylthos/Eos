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
            EOS_CORE_LOG_CRITICAL("Failed to init GLFW");
            exit(-1);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Initialised = true;

        EOS_CORE_LOG_INFO("Initialised GLFW");
    }

    Window& Window::create(const char* title)
    {
        m_Window = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y,
                title, nullptr, nullptr);

        m_Created = true;

        EOS_CORE_LOG_INFO("Created Window \"{}\" - Size {}:{}", title,
                m_WindowSize.x, m_WindowSize.y);

        return *this;
    }

    void Window::createSurface(VkInstance& instance, VkSurfaceKHR* surface) const
    {
        glfwCreateWindowSurface(instance, m_Window, nullptr, surface);
    }

    glm::vec2 Window::getSize()
    {
        reloadFramebufferSize();

        return m_WindowSize;
    }

    VkExtent2D Window::getExtent()
    { 
        reloadFramebufferSize();

        return { static_cast<uint32_t>(m_WindowSize.x),
                 static_cast<uint32_t>(m_WindowSize.y) };
    }

    VkViewport Window::getViewport()
    {
        reloadFramebufferSize();

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_WindowSize.x);
        viewport.height = static_cast<float>(m_WindowSize.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        return viewport;
    }

    VkRect2D Window::getScissor()
    {
        reloadFramebufferSize();

        VkRect2D scissor{};

        scissor.offset = { 0, 0 };
        scissor.extent = getExtent();

        return scissor;
    }

    void Window::reloadFramebufferSize()
    {
        int width, height;
        glfwGetFramebufferSize(m_Window, &width, &height);

        if (m_WindowSize.x != width || m_WindowSize.y != height)
        {
            m_WindowSize = { width, height };
        }
    }

    void Window::glfwErrorCallback(int, const char* errStr)
    {
        EOS_CORE_LOG_ERROR("{}", errStr);
    }
}
