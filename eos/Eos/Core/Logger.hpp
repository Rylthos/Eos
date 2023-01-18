#pragma once

#pragma warning(push, 0)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <spdlog/spdlog.h>
#pragma clang diagnostic pop
#pragma warning(pop)

#ifndef EOS_DISABLE_LOG
    #define EOS_LOG_TRACE(...)    spdlog::trace(__VA_ARGS__)
    #define EOS_LOG_DEBUG(...)    spdlog::debug(__VA_ARGS__)
    #define EOS_LOG_INFO(...)     spdlog::info(__VA_ARGS__)
    #define EOS_LOG_WARN(...)     spdlog::warn(__VA_ARGS__)
    #define EOS_LOG_ERROR(...)    spdlog::error(__VA_ARGS__)
    #define EOS_LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)
#else
    #define EOS_LOG_TRACE
    #define EOS_LOG_DEBUG
    #define EOS_LOG_INFO
    #define EOS_LOG_WARN
    #define EOS_LOG_ERROR
    #define EOS_LOG_CRITICAL
#endif
