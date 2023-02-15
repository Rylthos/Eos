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
        static void setupListener(std::shared_ptr<Window>& window);
        void addDispatcher(EventDispatcher* dispatcher);
        void removeDispatcher(EventDispatcher* dispatcher);
    private:
        static std::vector<EventDispatcher*> s_Dispatchers;
    private:
        static void glfwKeyCallback(GLFWwindow* window, int key, int scancode,
                int action, int mods);
        static void glfwMouseButtonCallback(GLFWwindow* window, int button,
                int action, int mods);
        static void glfwMouseMoveCallback(GLFWwindow* window, double x, double y);
        static void glfwScrollCallback(GLFWwindow* window, double x, double y);
        static void glfwWindowResizeCallback(GLFWwindow* window, int width, int height);

        template <typename T>
        static void dispatchEvent(const T& event)
        {
            for (EventDispatcher* dispatcher : s_Dispatchers)
            {
                dispatcher->dispatchEvent(event);
            }
        }
    };
}
