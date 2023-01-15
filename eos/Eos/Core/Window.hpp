#pragma once

#include "Core.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

namespace Eos
{
    class EOS_API Window
    {
    public:
        Window();
        ~Window();

        GLFWwindow* m_Window;

        void init();
        Window& create(const char* title);

        void createSurface(VkInstance& instance, VkSurfaceKHR* surface) const;

        void setWindowHint(int hint, int value) { glfwWindowHint(hint, value); }
        void setWindowAttrib(int attrib, int value)
            { glfwSetWindowAttrib(m_Window, attrib, value); }

        void setWindowSize(glm::vec2 windowSize) { m_WindowSize = windowSize; }

        GLFWwindow* getWindow() { return m_Window; }
        glm::vec2 getWindowSize() const { return m_WindowSize; }
        VkExtent2D getWindowExtent() const
            { return { static_cast<uint32_t>(m_WindowSize.x),
                         static_cast<uint32_t>(m_WindowSize.y) }; }

        VkViewport getViewport() const;
        VkRect2D getScissor() const;

        void setWindowShouldClose(bool value) { glfwSetWindowShouldClose(m_Window, value); }

        bool shouldClose() const { return glfwWindowShouldClose(m_Window); }
        bool isValid() const { return m_Created && m_Initialised; }
    private:
        glm::vec2 m_WindowSize;
        bool m_Created = false;
        bool m_Initialised = false;
    private:

        static void glfwErrorCallback(int, const char* errStr);
    };
}
