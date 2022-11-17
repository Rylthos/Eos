#pragma once

#include "Core.hpp"

#include <GLFW/glfw3.h>

namespace Eos
{
    class EOS_API Application
    {
    public:
        Application();
        virtual ~Application();

        void start();
    protected:
        GLFWwindow* m_Window;
    private:
        void mainLoop();

        virtual GLFWwindow* init();
        virtual void draw();
        virtual void update(float dt);
    };

    Application* createApplication();
}
