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

        void setInputMode(int mode, int value)
            { glfwSetInputMode(m_Window, mode, value); }

        void setWindowSize(glm::vec2 windowSize) { m_WindowSize = windowSize; }

        GLFWwindow* getWindow() { return m_Window; }
        glm::vec2 getSize();
        VkExtent2D getExtent();

        VkViewport getViewport();
        VkRect2D getScissor();

        void setWindowShouldClose(bool value) { glfwSetWindowShouldClose(m_Window, value); }

        bool shouldClose() const { return glfwWindowShouldClose(m_Window); }
        bool isValid() const { return m_Created && m_Initialised; }
    private:
        glm::ivec2 m_WindowSize;
        bool m_Created = false;
        bool m_Initialised = false;
    private:

        void reloadFramebufferSize();

        static void glfwErrorCallback(int, const char* errStr);
    };
}
