#pragma once

#include "EventCodes.hpp"

namespace Eos::Events
{
    struct Event
    {
        Type eventType;

        void* dataPointer = nullptr;

        Event() {}
    };

    struct KeyInputEvent : public Event
    {
        static constexpr Type eventType = Type::KEY_INPUT;
        Action action;
        Key key;
        Mods mods;

        KeyInputEvent(Action action, Key key, Mods mods)
            : action(action), key(key), mods(mods) {}
    };

    struct MousePressEvent : public Event
    {
        static constexpr Type eventType = Type::MOUSE_PRESS;
        Action action;
        MouseButton button;
        Mods mods;

        MousePressEvent(Action action, MouseButton button, Mods mods)
            : action(action), button(button), mods(mods) {}
    };

    struct MouseMoveEvent : public Event
    {
        static constexpr Type eventType = Type::MOUSE_MOVE;
        float xPos;
        float yPos;

        MouseMoveEvent(float xPos, float yPos)
            : xPos(xPos), yPos(yPos) {}
    };

    struct ScrollEvent : public Event
    {
        static constexpr Type eventType = Type::SCROLL;
        float xOff;
        float yOff;

        ScrollEvent(float xOff, float yOff)
            : xOff(xOff), yOff(yOff) {}
    };
}
