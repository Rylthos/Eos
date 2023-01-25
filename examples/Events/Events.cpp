#include <iostream>

#include "Eos/Eos.hpp"

struct Vertex
{
    glm::vec3 position;

    static Eos::VertexInputDescription getVertexDescription()
    {
        Eos::VertexInputDescription description;
        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.stride = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        description.bindings.push_back(binding);

        VkVertexInputAttributeDescription positionAttribute{};
        positionAttribute.binding = 0;
        positionAttribute.location = 0;
        positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        positionAttribute.offset = offsetof(Vertex, position);
        description.attributes.push_back(positionAttribute);

        return description;
    }
};

class Sandbox : public Eos::Application
{
public:
public:
    Sandbox(const Eos::ApplicationDetails& details)
        : Eos::Application(details) {}

    ~Sandbox() {}
private:
    VkPipeline m_Pipeline;
    Eos::IndexedMesh<Vertex, uint16_t> m_Mesh;

    float m_MouseX;
    float m_MouseY;
private:
    void windowInit() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("Events");
    }

    void postEngineInit() override
    {
        m_MainEventDispatcher.addCallback(&keyboardEvent, this);
        m_MainEventDispatcher.addCallback(&mouseMoveEvent, this);
        m_MainEventDispatcher.addCallback(&mousePressEvent, this);

        std::vector<Vertex> vertices = {
            { { -1.0f,  1.0f, 0.0f} },
            { {  1.0f,  1.0f, 0.0f} },
            { {  0.0f, -1.0f, 0.0f} },
        };
        m_Mesh.setVertices(vertices);

        std::vector<uint16_t> indices = {
            0, 1, 2
        };
        m_Mesh.setIndices(indices);

        m_Mesh.create();

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "res/Events/Shaders/Events.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "res/Events/Shaders/Events.frag.spv");

        m_Engine->createPipelineBuilder()
            .setShaderStages(shader.getShaderStages())
            .setVertexInputInfo(Vertex::getVertexDescription())
            .setViewports({ m_Window.getViewport() })
            .setScissors({ m_Window.getScissor() })
            .build(m_Pipeline);

        shader.clearModules();
    }

    void draw(VkCommandBuffer cmd) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &m_Mesh.getVertexBuffer()->buffer,
                &offset);
        vkCmdBindIndexBuffer(cmd, m_Mesh.getIndexBuffer()->buffer,
                0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(cmd, static_cast<uint32_t>(m_Mesh.getIndices()->size()),
                1, 0, 0, 0);
    }

    void update(double dt) override {}

    static bool keyboardEvent(const Eos::Events::KeyInputEvent* event)
    {
        Sandbox* sb = (Sandbox*)event->dataPointer;

        if (event->key == Eos::Events::Key::KEY_ESCAPE &&
                event->action == Eos::Events::Action::PRESS)
        {
            sb->m_Window.setWindowShouldClose(true);
        }

        if (static_cast<int>(event->key) >= 65 && static_cast<int>(event->key) <= 90)
        {
            if (event->action == Eos::Events::Action::PRESS)
            {
                EOS_CORE_LOG_INFO("{}", (char)(static_cast<int>(event->key)));
            }
        }

        return true;
    }

    static bool mouseMoveEvent(const Eos::Events::MouseMoveEvent* event)
    {
        Sandbox* sb = (Sandbox*)event->dataPointer;

        sb->m_MouseX = event->xPos;
        sb->m_MouseY = event->yPos;

        return true;
    }

    static bool mousePressEvent(const Eos::Events::MousePressEvent* event)
    {
        Sandbox* sb = (Sandbox*)event->dataPointer;

        if (event->action == Eos::Events::Action::PRESS)
        {
            EOS_CORE_LOG_INFO("{} {}", sb->m_MouseX, sb->m_MouseY);
        }

        return true;
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "Events";

    return new Sandbox(details);
}
