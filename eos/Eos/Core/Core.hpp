#pragma once

#if defined(EOS_PLATFORM_WINDOWS)
    #ifdef EOS_BUILD_DLL
        #define EOS_API __declspec(dllexport)
    #else
        #define EOS_API __declspec(dllimport)
    #endif
#elif defined(EOS_PLATFORM_LINUX)
    #ifdef EOS_BUILD_DLL
        #define EOS_API __attribute__((visibility("default")))
    #else
        #define EOS_API
    #endif
#else
    #define EOS_API
    #pragma warning Unknown import/export semantics.
#endif
