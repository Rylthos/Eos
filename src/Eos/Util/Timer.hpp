#pragma once

#include "Eos/EosPCH.hpp"

#include <chrono>

namespace Eos
{
    class EOS_API Timer
    {
    public:
        Timer() {}
        ~Timer() {}

        void start();
        void tick();
        void end();

        bool running() { return m_Running; }

        std::chrono::time_point<std::chrono::steady_clock>
            getStartTime() { return m_StartTime; }
        std::chrono::time_point<std::chrono::steady_clock>
            getEndTime() { return m_EndTime; }

        double timeElapsed();
        double timeElapsedMilli();
    private:
        std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
        std::chrono::time_point<std::chrono::steady_clock> m_EndTime;
        std::chrono::duration<double> m_DeltaTime;

        bool m_Running = false;
    };
}
