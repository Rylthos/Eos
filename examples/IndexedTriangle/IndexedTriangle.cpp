#include <iostream>

#include "Eos/Eos.hpp"
#include "Eos/Core/EntryPoint.hpp"

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
    Sandbox(const Eos::ApplicationDetails& details)
        : Eos::Application(details) {}

    ~Sandbox() {}
private:
    VkPipeline m_Pipeline;
    Eos::IndexedMesh<Vertex, uint16_t> m_Mesh;
private:
    void windowInit() override
    {
        m_Window->setWindowSize({ 500, 500 });

        m_Window->create("Indexed Triangle");
    }

    void postEngineInit() override
    {
        std::vector<Vertex> vertices = {
            { { -1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { {  1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
            { {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
        };
        m_Mesh.setVertices(vertices);

        std::vector<uint16_t> indices = {
            0, 1, 2,
            1, 2, 3
        };
        m_Mesh.setIndices(indices);

        m_Mesh.create();

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "res/IndexedTriangle/Shaders/IndexedTriangle.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "res/IndexedTriangle/Shaders/IndexedTriangle.frag.spv");

        m_Engine->createPipelineBuilder()
            .setShaderStages(shader.getShaderStages())
            .setVertexInputInfo(Vertex::getVertexDescription())
            .setViewports({ m_Window->getViewport() })
            .setScissors({ m_Window->getScissor() })
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
