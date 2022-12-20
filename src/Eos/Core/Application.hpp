#pragma once

#include "Core.hpp"

#include "Window.hpp"
#include "Logger.hpp"
#include "../Engine/Engine.hpp"
#include "../Engine/Mesh.hpp"
#include "../Util/Types.hpp"

#include <GLFW/glfw3.h>

#include <string>

#include <unordered_map>

#include "../Events/EventListener.hpp"
#include "../Events/EventDispatcher.hpp"

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
    protected:
        template<VertexTemplate T>
        void uploadMesh(Mesh<T>& mesh) { m_Engine->createMesh(mesh); }

        template<VertexTemplate T, typename I>
            requires std::is_integral<I>::value
        void uploadIndexedMesh(IndexedMesh<T, I>& mesh) { m_Engine->createIndexedMesh(mesh); }

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
