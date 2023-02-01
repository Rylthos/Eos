#include "Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Eos
{
    std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

    void Logger::init()
    {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        sinks[0]->set_pattern("[%d/%m/%C %H:%M:%S.%e] [%n] [%^%l%$] %v");

        s_CoreLogger = std::make_shared<spdlog::logger>("Core", begin(sinks), end(sinks));

        s_ClientLogger = std::make_shared<spdlog::logger>("Client", begin(sinks), end(sinks));
    }

    void Logger::enable()
    {
        s_CoreLogger->set_level(spdlog::level::info);
        s_ClientLogger->set_level(spdlog::level::info);
    }

    void Logger::disable()
    {
        s_CoreLogger->set_level(spdlog::level::off);
        s_ClientLogger->set_level(spdlog::level::off);
    }
}
