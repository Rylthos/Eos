#pragma once

#include "Core.hpp"

#include "Window.hpp"
#include "../Engine/Engine.hpp"
#include "../Engine/Mesh.hpp"
#include "../Util/Types.hpp"

#include <GLFW/glfw3.h>

#include <string>

#include <unordered_map>

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
        template<VertexDescription T>
        void uploadMesh(Mesh<T>& mesh) { m_Engine->createMesh(mesh); }
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
