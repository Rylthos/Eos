#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Core/Window.hpp"
#include "Eos/Core/Logger.hpp"
#include "Eos/Core/Timer.hpp"

#include "Eos/Engine/Engine.hpp"
#include "Eos/Engine/Mesh.hpp"
#include "Eos/Engine/Types.hpp"

#include <GLFW/glfw3.h>

#include <string>

#include <unordered_map>

#include "Eos/Events/EventListener.hpp"
#include "Eos/Events/EventDispatcher.hpp"

namespace Eos
{
    struct ApplicationDetails
    {
        std::string name;
        bool enableVsync = true;
        bool enableWindowResizing = false;
        bool customRenderpass = false;
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
        Timer m_FrameTimer;

        Events::EventListener m_MainEventListener;
        Events::EventDispatcher m_MainEventDispatcher;
    private:
        ApplicationDetails m_Details;
    private:
        void mainLoop();

        virtual void windowInit();
        virtual void renderPassInit(RenderPass& renderPass);
        virtual void postEngineInit();

        virtual void draw(VkCommandBuffer cmd);
        virtual void update(double dt);
    };

    Application* createApplication();
}
