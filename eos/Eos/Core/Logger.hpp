#pragma once

#include "Eos/Core/Core.hpp"

#pragma warning(push, 0)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <spdlog/spdlog.h>
#pragma clang diagnostic pop
#pragma warning(pop)

namespace Eos
{
    class EOS_API Logger
    {
    public:
        static void init();

        static void enable();
        static void disable();

        static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_ClientLogger; }
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

#ifndef EOS_DISABLE_LOG
    #define EOS_DISABLE_LOGGER() Eos::Logger::disable();
    #define EOS_ENABLE_LOGGER()  Eos::Logger::enable();

    #define EOS_CORE_LOG_TRACE(...)    Eos::Logger::getCoreLogger()->trace(__VA_ARGS__)
    #define EOS_CORE_LOG_DEBUG(...)    Eos::Logger::getCoreLogger()->debug(__VA_ARGS__)
    #define EOS_CORE_LOG_INFO(...)     Eos::Logger::getCoreLogger()->info(__VA_ARGS__)
    #define EOS_CORE_LOG_WARN(...)     Eos::Logger::getCoreLogger()->warn(__VA_ARGS__)
    #define EOS_CORE_LOG_ERROR(...)    Eos::Logger::getCoreLogger()->error(__VA_ARGS__)
    #define EOS_CORE_LOG_CRITICAL(...) Eos::Logger::getCoreLogger()->critical(__VA_ARGS__)

    #define EOS_LOG_TRACE(...)    Eos::Logger::getClientLogger()->trace(__VA_ARGS__)
    #define EOS_LOG_DEBUG(...)    Eos::Logger::getClientLogger()->debug(__VA_ARGS__)
    #define EOS_LOG_INFO(...)     Eos::Logger::getClientLogger()->info(__VA_ARGS__)
    #define EOS_LOG_WARN(...)     Eos::Logger::getClientLogger()->warn(__VA_ARGS__)
    #define EOS_LOG_ERROR(...)    Eos::Logger::getClientLogger()->error(__VA_ARGS__)
    #define EOS_LOG_CRITICAL(...) Eos::Logger::getClientLogger()->critical(__VA_ARGS__)
#else
    #define EOS_CORE_LOG_TRACE
    #define EOS_CORE_LOG_DEBUG
    #define EOS_CORE_LOG_INFO
    #define EOS_CORE_LOG_WARN
    #define EOS_CORE_LOG_ERROR
    #define EOS_CORE_LOG_CRITICAL

    #define EOS_LOG_TRACE
    #define EOS_LOG_DEBUG
    #define EOS_LOG_INFO
    #define EOS_LOG_WARN
    #define EOS_LOG_ERROR
    #define EOS_LOG_CRITICAL
#endif
