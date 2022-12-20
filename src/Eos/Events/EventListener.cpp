#include "EventListener.hpp"

#include "../Core/Logger.hpp"

namespace Eos::Events
{
    void EventListener::addListeners(Window& window)
    {
        glfwSetKeyCallback(window.getWindow(), glfwKeyCallback);
        glfwSetMouseButtonCallback(window.getWindow(), glfwMouseButtonCallback);
        glfwSetCursorPosCallback(window.getWindow(), glfwMouseMoveCallback);
        glfwSetScrollCallback(window.getWindow(), glfwScrollCallback);

        EOS_LOG_INFO("Setup Event Listeners");
    }

    void EventListener::glfwKeyCallback(GLFWwindow* window, int key, int scancode,
            int action, int mods)
    {
        KeyInputEvent event {
            static_cast<Action>(action),
            static_cast<Key>(key),
            static_cast<Mods>(mods)
        };

        EventDispatcher::dispatchEvent(event);
        EOS_LOG_TRACE("Dispatched Key Input Event");
    }

    void EventListener::glfwMouseButtonCallback(GLFWwindow* window, int button,
            int action, int mods)
    {
        MousePressEvent event {
            static_cast<Action>(action),
            static_cast<MouseButton>(button),
            static_cast<Mods>(mods)
        };

        EventDispatcher::dispatchEvent(event);
        EOS_LOG_TRACE("Dispatched Mouse Button Event");
    }

    void EventListener::glfwMouseMoveCallback(GLFWwindow* window, double x, double y)
    {
        MouseMoveEvent event {
            static_cast<float>(x),
            static_cast<float>(y)
        };

        EventDispatcher::dispatchEvent(event);
        EOS_LOG_TRACE("Dispatched Mouse Move Event");
    }

    void EventListener::glfwScrollCallback(GLFWwindow* window, double x, double y)
    {
        ScrollEvent event {
            static_cast<float>(x),
            static_cast<float>(y)
        };

        EventDispatcher::dispatchEvent(event);
        EOS_LOG_TRACE("Dispatched Scroll Event");
    }
}
