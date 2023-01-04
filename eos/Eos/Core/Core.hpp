#pragma once

#ifdef EOS_PLATFORM_WINDOWS
    #ifdef EOS_BUILD_DLL
        #define EOS_API __declspec(dllexport)
    #else
        #define EOS_API __declspec(dllimport)
    #endif
#else
    #error Eos currently only supports windows!
#endif
