#pragma once

#include "../Core/Core.hpp"

#include "Events.hpp"

#include <unordered_map>
#include <vector>

#include <functional>

namespace Eos::Events
{
    template <typename T>
    concept EventTemplate = std::is_base_of<Event, T>();

    typedef bool(*EventFunction)(const Event*);

    class EOS_API EventDispatcher
    {
    public:
        template <EventTemplate T>
        static void addCallback(bool(*function)(const T*))
        {
            EventFunction castFunction =
                reinterpret_cast<EventFunction>(function);

            s_FunctionCallbacks[T::eventType].push_back(castFunction);
        }

        template <EventTemplate T>
        static void dispatchEvent(const T& event)
        {
            const auto& functions = s_FunctionCallbacks[event.eventType];

            for (EventFunction func : functions)
            {
                bool (*castFunc)(const T*) =
                    reinterpret_cast<bool(*)(const T*)>(func);

                if (castFunc(&event))
                {
                    break;
                }
            }
        }
    private:
        static std::unordered_map<Type, std::vector<EventFunction>> s_FunctionCallbacks;
    };
}
