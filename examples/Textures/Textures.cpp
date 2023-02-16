#include "Eos/Eos.hpp"
#include "Eos/Core/EntryPoint.hpp"

#include <vulkan/vulkan_core.h>

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;

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

        VkVertexInputAttributeDescription uvAttribute{};
        uvAttribute.binding = 0;
        uvAttribute.location = 1;
        uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
        uvAttribute.offset = offsetof(Vertex, uv);
        description.attributes.push_back(uvAttribute);

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

    VkDescriptorSet m_Set;
    VkDescriptorSetLayout m_SetLayout;

    Eos::Texture2D m_Texture;

    Eos::IndexedMesh<Vertex, uint16_t> m_Mesh;
private:
    void windowInit() override
    {
        m_Window->setWindowSize({ 500, 500 });
        m_Window->create("Texture");
    }

    void postEngineInit() override
    {
        std::vector<Vertex> vertices = {
            { { -1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f } },
            { {  1.0f,  1.0f, 1.0f }, { 1.0f, 0.0f } },
            { { -1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f } },
        };

        std::vector<uint16_t> indices = {
            0, 1, 2,
            1, 2, 3
        };

        m_Mesh.setVertices(vertices);
        m_Mesh.setIndices(indices);

        m_Mesh.create();

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT,
                "res/Textures/Shaders/Texture.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT,
                "res/Textures/Shaders/Texture.frag.spv");

        m_Texture.loadFromFile("res/Textures/Textures/Swirl.png");
        m_Texture.addToDeletionQueue(Eos::GlobalData::getDeletionQueue());

        VkDescriptorImageInfo imageBI;
        imageBI.sampler = m_Texture.sampler.value();
        imageBI.imageView = m_Texture.imageView;
        imageBI.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        m_Engine->createDescriptorBuilder()
            .bindImage(0, &imageBI,
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build(m_Set, m_SetLayout);

        VkPipelineLayoutCreateInfo layoutInfo = Eos::Pipeline::pipelineLayoutCreateInfo();
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &m_SetLayout;

        m_Engine->createPipelineBuilder()
            .setShaderStages(shader.getShaderStages())
            .setVertexInputInfo(Vertex::getVertexDescription())
            .setViewports({ m_Window->getViewport() })
            .setScissors({ m_Window->getScissor() })
            .build(m_Pipeline, m_PipelineLayout, layoutInfo);
    }

    void draw(VkCommandBuffer cmd) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &m_Mesh.getVertexBuffer()->buffer, &offset);
        vkCmdBindIndexBuffer(cmd, m_Mesh.getIndexBuffer()->buffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout,
                0, 1, &m_Set, 0, nullptr);

        vkCmdDrawIndexed(cmd, m_Mesh.getIndices()->size(), 1, 0, 0, 0);
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "Texture";

    return new Sandbox(details);
}
