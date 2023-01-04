#include <iostream>

#include "Eos/Eos.hpp"
#include "Eos/Util/PipelineCreation.hpp"

#include <GLFW/glfw3.h>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 colour;

    static Eos::VertexInputDescription getVertexDescription()
    {
        Eos::VertexInputDescription description;
        VkVertexInputBindingDescription mainBinding{};
        mainBinding.binding = 0;
        mainBinding.stride = sizeof(Vertex);
        mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        description.bindings.push_back(mainBinding);

        VkVertexInputAttributeDescription positionAttribute{};
        positionAttribute.binding = 0;
        positionAttribute.location = 0;
        positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        positionAttribute.offset = offsetof(Vertex, position);
        description.attributes.push_back(positionAttribute);

        VkVertexInputAttributeDescription colourAttribute{};
        colourAttribute.binding = 0;
        colourAttribute.location = 1;
        colourAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        colourAttribute.offset = offsetof(Vertex, colour);
        description.attributes.push_back(colourAttribute);

        return description;
    }
};

class Sandbox : public Eos::Application
{
public:
    float mouseX, mouseY;
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

        m_Window.create("Indexed Triangle");
    }

    void postInit() override
    {
        std::vector<Vertex> vertices = {
            { { -1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { {  1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { {  0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        };
        m_Mesh.setVertices(vertices);

        std::vector<uint16_t> indices = {
            0, 1, 2
        };
        m_Mesh.setIndices(indices);

        uploadIndexedMesh(m_Mesh);

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "Shaders/IndexedTriangle.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "Shaders/IndexedTriangle.frag.spv");
        m_Engine->getPipelineBuilder()->shaderStages = shader.getShaderStages();

        m_Engine->getPipelineBuilder()->addVertexInputInfo(Vertex::getVertexDescription());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_Window.getWindowExtent().width;
        viewport.height = (float)m_Window.getWindowExtent().height;
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

    void update(double dt) override
    {
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "Indexed Triangle";

    return new Sandbox(details);
}
