#pragma once

#include "Core.hpp"

#include "Window.hpp"
#include "../Engine/Engine.hpp"

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
        Window m_Window;
        Engine* m_Engine;
    private:
        ApplicationDetails m_Details;
    private:
        void mainLoop();

        virtual void init();
        virtual void postInit();

        virtual void draw(VkCommandBuffer cmd);
        virtual void update(float dt);
    };

    Application* createApplication();
}
