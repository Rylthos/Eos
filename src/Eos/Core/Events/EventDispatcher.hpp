#pragma once

#include "../Core.hpp"

#include "Events.hpp"

#include <unordered_map>
#include <vector>

#include <functional>

namespace Eos
{
    typedef std::function<bool(Event*)> EventFunction;

    class EOS_API EventDispatcher
    {
    public:
        static void addCallback(EventType type, EventFunction function);
        static void dispatchEvent(Event& event);
    private:
        static std::unordered_map<EventType, std::vector<EventFunction>> s_FunctionCallbacks;
    };
}
