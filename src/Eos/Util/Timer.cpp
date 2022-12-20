#include "Timer.hpp"

#include <GLFW/glfw3.h>

namespace Eos
{
    void Timer::start()
    {
        if (m_Running) EOS_LOG_ERROR("Timer is already running");

        m_StartTime = std::chrono::steady_clock::now();
        m_Running = true;
    }

    void Timer::tick()
    {
        if (!m_Running) EOS_LOG_ERROR("Timer is not running");

        m_EndTime = std::chrono::steady_clock::now();
        m_DeltaTime = m_EndTime - m_StartTime;
        m_StartTime = m_EndTime;
    }

    void Timer::end()
    {
        if (!m_Running) EOS_LOG_ERROR("Timer is not running");

        m_EndTime = std::chrono::steady_clock::now();
        m_Running = false;
    }

    double Timer::timeElapsed()
    {
        return m_DeltaTime.count();
    }

    double Timer::timeElapsedMilli()
    {
        return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>
            (m_DeltaTime).count();
    }
}
