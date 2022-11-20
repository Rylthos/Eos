#pragma once

#include "Core.hpp"

#include <GLFW/glfw3.h>

#include <string>

namespace Eos
{
    struct ApplicationDetails
    {
        std::string name;
    };

    class EOS_API Application
    {
    public:
        Application(const ApplicationDetails& details);
        virtual ~Application();

        void start();

    protected:
        GLFWwindow* m_Window;
    private:
        ApplicationDetails m_Details;
    private:
        void mainLoop();

        virtual GLFWwindow* init();
        virtual void draw();
        virtual void update(float dt);
    };

    Application* createApplication();
}
