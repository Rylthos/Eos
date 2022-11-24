#include "Window.hpp"

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
            std::cout << "Failed to init GLFW\n";
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Initialised = true;
    }

    Window& Window::create(const char* title)
    {
        m_Window = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y,
                title, nullptr, nullptr);

        m_Created = true;

        return *this;
    }

    void Window::createSurface(VkInstance& instance, VkSurfaceKHR* surface) const
    {
        glfwCreateWindowSurface(instance, m_Window, nullptr, surface);
    }

    void Window::glfwErrorCallback(int, const char* errStr)
    {
        std::cerr << errStr << "\n";
    }
}
