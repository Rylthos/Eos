#pragma once

#include "../Core.hpp"
#include "../Window.hpp"

#include "Events.hpp"
#include "EventDispatcher.hpp"

namespace Eos
{
    class EOS_API EventListener
    {
    public:
        static void addListeners(Window& window);
    private:
        static void glfwKeyCallback(GLFWwindow* window, int key, int scancode,
                int action, int mods);
    };
}
