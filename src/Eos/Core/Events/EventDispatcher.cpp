#include "EventDispatcher.hpp"

namespace Eos
{
    std::unordered_map<EventType, std::vector<EventFunction>> EventDispatcher::s_FunctionCallbacks;

    void EventDispatcher::addCallback(EventType type, EventFunction function)
    {
        s_FunctionCallbacks[type].push_back(function);
    }

    void EventDispatcher::dispatchEvent(Event& event)
    {
        std::vector<EventFunction>& functions = s_FunctionCallbacks[event.eventType];

        for (const EventFunction& func : functions)
        {
            if (func(&event)) // Event was consumed
            {
                break;
            }
        }
    }
};
