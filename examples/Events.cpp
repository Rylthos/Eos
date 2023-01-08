#include <iostream>

#include "Eos/Eos.hpp"
#include "Eos/Util/PipelineCreation.hpp"

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
    float mouseX;
    float mouseY;
public:
    Sandbox(const Eos::ApplicationDetails& details)
        : Eos::Application(details) {}

    ~Sandbox() {}
private:
    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
    Eos::IndexedMesh<Vertex, uint16_t> m_Mesh;
private:
    void init() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("Events");
    }

    void postInit() override
    {
        m_MainEventDispatcher.addCallback(&keyboardEvent);
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

        uploadIndexedMesh(m_Mesh);

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "res/Events/Shaders/Events.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "res/Events/Shaders/Events.frag.spv");
        m_Engine->getPipelineBuilder()->shaderStages = shader.getShaderStages();

        m_Engine->getPipelineBuilder()->addVertexInputInfo(Vertex::getVertexDescription());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_Window.getWindowExtent().width);
        viewport.height = static_cast<float>(m_Window.getWindowExtent().height);
        m_Engine->getPipelineBuilder()->viewport = viewport;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_Window.getWindowExtent();
        m_Engine->getPipelineBuilder()->scissor = scissor;

        m_PipelineLayout = m_Engine->setupPipelineLayout();
        m_Pipeline = m_Engine->setupPipeline(m_PipelineLayout);

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
        if (static_cast<int>(event->key) >= 65 && static_cast<int>(event->key) <= 90)
        {
            if (event->action == Eos::Events::Action::PRESS)
            {
                EOS_LOG_INFO("{}", (char)(static_cast<int>(event->key)));
            }
        }

        return true;
    }

    static bool mouseMoveEvent(const Eos::Events::MouseMoveEvent* event)
    {
        Sandbox* sb = (Sandbox*)event->dataPointer;

        sb->mouseX = event->xPos;
        sb->mouseY = event->yPos;

        return true;
    }

    static bool mousePressEvent(const Eos::Events::MousePressEvent* event)
    {
        Sandbox* sb = (Sandbox*)event->dataPointer;

        if (event->action == Eos::Events::Action::PRESS)
        {
            EOS_LOG_INFO("{} {}", sb->mouseX, sb->mouseY);
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