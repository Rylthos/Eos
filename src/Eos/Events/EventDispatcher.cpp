#include "EventDispatcher.hpp"

namespace Eos::Events
{
    std::unordered_map<Type, std::vector<EventFunction>>
        EventDispatcher::s_FunctionCallbacks;
};
