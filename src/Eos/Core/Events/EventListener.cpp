#include "EventListener.hpp"

#include "../Logger.hpp"

namespace Eos
{
    void EventListener::addListeners(Window& window)
    {
        glfwSetKeyCallback(window.getWindow(), glfwKeyCallback);

        EOS_LOG_INFO("Setup Event Listeners");
    }

    void EventListener::glfwKeyCallback(GLFWwindow* window, int key, int scancode,
            int action, int mods)
    {
        KeyEvent event {
            static_cast<ActionType>(action),
            static_cast<KeyType>(key),
            static_cast<ModType>(mods)
        };

        EventDispatcher::dispatchEvent(event);
        EOS_LOG_TRACE("Dispatched Key Input Event");
    }
}
