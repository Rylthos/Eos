#pragma once

#include "Core.hpp"

namespace Eos
{
    class EOS_API Application
    {
    public:
        Application();
        virtual ~Application();

        void start();
        void run();
    };
}
