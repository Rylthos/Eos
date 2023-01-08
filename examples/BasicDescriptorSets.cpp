#include <iostream>

#include "Eos/Eos.hpp"
#include "Eos/Util/PipelineCreation.hpp"

#include <GLFW/glfw3.h>

struct Colour
{
    glm::vec4 colour;
};

struct Vertex
{
    glm::vec3 position;

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
    VkPipelineLayout m_PipelineLayout;
    Eos::IndexedMesh<Vertex, uint16_t> m_Mesh;

    VkDescriptorSet m_ColourSet;
    VkDescriptorSetLayout m_ColourSetLayout;
    Eos::Buffer m_ColourBuffer;
private:
    void init() override
    {
        m_Window.setWindowSize({ 500, 500 });

        m_Window.create("Colour Changing Triangle");
    }

    void postInit() override
    {
        std::vector<Vertex> vertices = {
            { { -1.0f,  1.0f, 0.0f } },
            { {  1.0f,  1.0f, 0.0f } },
            { {  0.0f, -1.0f, 0.0f } },
        };
        m_Mesh.setVertices(vertices);

        std::vector<uint16_t> indices = {
            0, 1, 2
        };
        m_Mesh.setIndices(indices);

        uploadIndexedMesh(m_Mesh);

        m_ColourBuffer = m_Engine->createBuffer(sizeof(Colour), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VMA_MEMORY_USAGE_CPU_TO_GPU);
        m_Engine->getDeletionQueue()->pushFunction([&]()
                {
                    vmaDestroyBuffer(*m_Engine->getAllocator(), m_ColourBuffer.buffer,
                            m_ColourBuffer.allocation);
                });

        VkDescriptorBufferInfo colourInfo{};
        colourInfo.buffer = m_ColourBuffer.buffer;
        colourInfo.offset = 0;
        colourInfo.range = sizeof(Colour);

        m_Engine->createDescriptorBuilder()
            .bindBuffer(0, &colourInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                VK_SHADER_STAGE_VERTEX_BIT)
            .build(m_ColourSet, m_ColourSetLayout);

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "res/BasicDescriptorSets/Shaders/BasicDescriptorSets.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "res/BasicDescriptorSets/Shaders/BasicDescriptorSets.frag.spv");
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

        VkPipelineLayoutCreateInfo info = m_Engine->createPipelineLayoutCreateInfo();
        info.setLayoutCount = 1;
        info.pSetLayouts = &m_ColourSetLayout;

        m_PipelineLayout = m_Engine->setupPipelineLayout(info);

        m_Pipeline = m_Engine->setupPipeline(m_PipelineLayout);

        shader.clearModules();

        Colour c;
        c.colour = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
        void* data;
        vmaMapMemory(*m_Engine->getAllocator(), m_ColourBuffer.allocation, &data);
            memcpy(data, &c, sizeof(Colour));
        vmaUnmapMemory(*m_Engine->getAllocator(), m_ColourBuffer.allocation);
    }

    void draw(VkCommandBuffer cmd) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &m_Mesh.getVertexBuffer()->buffer,
                &offset);
        vkCmdBindIndexBuffer(cmd, m_Mesh.getIndexBuffer()->buffer,
                0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout,
                0, 1, &m_ColourSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, static_cast<uint32_t>(m_Mesh.getIndices()->size()),
                1, 0, 0, 0);
    }

    void update(double dt) override
    {
        static double time = 0.0f;
        time += dt;

        Colour c;
        c.colour = glm::vec4(fmax(sin(time), 0.0f), fmax(cos(time), 0.0f), 0.0f, 1.0f);
        void* data;
        vmaMapMemory(*m_Engine->getAllocator(), m_ColourBuffer.allocation, &data);
            memcpy(data, &c, sizeof(Colour));
        vmaUnmapMemory(*m_Engine->getAllocator(), m_ColourBuffer.allocation);
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "Colour Changing Triangle";

    return new Sandbox(details);
}
