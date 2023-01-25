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
}
