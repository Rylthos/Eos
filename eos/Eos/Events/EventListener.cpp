#include "EventListener.hpp"

namespace Eos::Events
{
    std::vector<EventDispatcher*> EventListener::s_Dispatchers;

    void EventListener::setupListener(std::shared_ptr<Window>& window)
    {
        glfwSetKeyCallback(window->getWindow(), glfwKeyCallback);
        glfwSetMouseButtonCallback(window->getWindow(), glfwMouseButtonCallback);
        glfwSetCursorPosCallback(window->getWindow(), glfwMouseMoveCallback);
        glfwSetScrollCallback(window->getWindow(), glfwScrollCallback);
        glfwSetWindowSizeCallback(window->getWindow(), glfwWindowResizeCallback);

        EOS_CORE_LOG_INFO("Setup Event Listeners");
    }

    void EventListener::addDispatcher(EventDispatcher* eventDispatcher)
    {
        s_Dispatchers.push_back(eventDispatcher);

        EOS_CORE_LOG_INFO("Added Event Listener");
    }

    void EventListener::removeDispatcher(EventDispatcher* eventDispatcher)
    {
        for (int i = 0; i < s_Dispatchers.size(); i++)
        {
            if (s_Dispatchers[i] == eventDispatcher)
            {
                s_Dispatchers.erase(s_Dispatchers.begin() + i);
                return;
            }
        }
    }

    void EventListener::glfwKeyCallback(GLFWwindow* window, int key, int scancode,
            int action, int mods)
    {
        KeyInputEvent event {
            static_cast<Action>(action),
            static_cast<Key>(key),
            static_cast<Mods>(mods)
        };

        dispatchEvent(event);

        EOS_CORE_LOG_TRACE("Dispatched Key Input Event");
    }

    void EventListener::glfwMouseButtonCallback(GLFWwindow* window, int button,
            int action, int mods)
    {
        MousePressEvent event {
            static_cast<Action>(action),
            static_cast<MouseButton>(button),
            static_cast<Mods>(mods)
        };

        dispatchEvent(event);

        EOS_CORE_LOG_TRACE("Dispatched Mouse Button Event");
    }

    void EventListener::glfwMouseMoveCallback(GLFWwindow* window, double x, double y)
    {
        MouseMoveEvent event {
            static_cast<float>(x),
            static_cast<float>(y)
        };

        dispatchEvent(event);

        EOS_CORE_LOG_TRACE("Dispatched Mouse Move Event");
    }

    void EventListener::glfwScrollCallback(GLFWwindow* window, double x, double y)
    {
        ScrollEvent event {
            static_cast<float>(x),
            static_cast<float>(y)
        };

        dispatchEvent(event);

        EOS_CORE_LOG_TRACE("Dispatched Scroll Event");
    }

    void EventListener::glfwWindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        WindowResizeEvent event = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        dispatchEvent(event);

        EOS_CORE_LOG_TRACE("Dispatched Window Resize Event");
    }
}
