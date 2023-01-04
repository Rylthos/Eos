#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Core/Window.hpp"

#include "Eos/Events/Events.hpp"
#include "Eos/Events/EventDispatcher.hpp"

namespace Eos::Events
{
    class EOS_API EventListener
    {
    public:
        void addListeners(Window& window, EventDispatcher* dispatcher);
    private:
        static void glfwKeyCallback(GLFWwindow* window, int key, int scancode,
                int action, int mods);
        static void glfwMouseButtonCallback(GLFWwindow* window, int button,
                int action, int mods);
        static void glfwMouseMoveCallback(GLFWwindow* window, double x, double y);
        static void glfwScrollCallback(GLFWwindow* window, double x, double y);
    };
}
