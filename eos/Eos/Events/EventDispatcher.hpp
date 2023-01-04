#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Events/Events.hpp"

#include <functional>

namespace Eos::Events
{
    template <typename T>
    concept EventTemplate = std::is_base_of<Event, T>();

    class EOS_API EventDispatcher
    {
        typedef bool(*EventFunction)(const Event*);
        typedef std::pair<EventFunction, void*> FunctionPair;

    public:
        template <EventTemplate T>
        void addCallback(bool(*function)(const T*), void* dataPointer = nullptr)
        {
            EventFunction castFunction =
                reinterpret_cast<EventFunction>(function);

            s_FunctionCallbacks[T::eventType].push_back(
                    std::make_pair(castFunction, dataPointer));
        }

        template <EventTemplate T>
        void dispatchEvent(const T& event)
        {
            const auto& functions = s_FunctionCallbacks[event.eventType];

            for (FunctionPair func : functions)
            {
                bool (*castFunc)(const T*) =
                    reinterpret_cast<bool(*)(const T*)>(func.first);

                T modifiedEvent = event;
                modifiedEvent.dataPointer = func.second;

                if (castFunc(&modifiedEvent))
                {
                    break;
                }
            }
        }
    private:
        std::unordered_map<Type, std::vector<FunctionPair>> s_FunctionCallbacks;
    };
}
