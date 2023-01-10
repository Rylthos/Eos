#include "Eos/Eos.hpp"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

struct Data
{
    glm::mat4 projection;
    glm::mat4 model;
};

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
    float velX = 0.0f;
    float velY = 0.0f;
    float posX = 250.0f;
    float posY = 250.0f;
public:
    Sandbox(const Eos::ApplicationDetails& details)
        : Eos::Application(details) {}

    ~Sandbox() {}
private:
    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;

    VkDescriptorSet m_DataDescriptorSet;
    VkDescriptorSetLayout m_DataDescriptorSetLayout;
    Eos::Buffer m_DataBuffer;

    Data m_Data;

    Eos::IndexedMesh<Vertex, uint16_t> m_Mesh;
private:
    void init() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("Moving Square");
    }

    void postInit() override
    {
        m_MainEventDispatcher.addCallback(&keyboardEvent, this);

        std::vector<Vertex> vertices = {
            { { -1.0f,  1.0f, 0.0f}, { 1.0f, 0.0, 0.0 } },
            { {  1.0f,  1.0f, 0.0f}, { 1.0f, 0.0, 0.0 } },
            { { -1.0f, -1.0f, 0.0f}, { 1.0f, 0.0, 0.0 } },
            { {  1.0f, -1.0f, 0.0f}, { 1.0f, 0.0, 0.0 } },
        };

        std::vector<uint16_t> indices = {
            0, 1, 2,
            1, 2, 3
        };

        m_Mesh.setVertices(vertices);
        m_Mesh.setIndices(indices);

        uploadIndexedMesh(m_Mesh);

        // Setup descriptor set
        m_DataBuffer = m_Engine->createBuffer(sizeof(Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VMA_MEMORY_USAGE_CPU_TO_GPU);
        VkDescriptorBufferInfo dataInfo{};
        dataInfo.buffer = m_DataBuffer.buffer;
        dataInfo.offset = 0;
        dataInfo.range = sizeof(Data);

        m_Engine->createDescriptorBuilder()
            .bindBuffer(0, &dataInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT)
            .build(m_DataDescriptorSet, m_DataDescriptorSetLayout);

        m_Engine->getDeletionQueue()->pushFunction([&]()
                {
                    vmaDestroyBuffer(*m_Engine->getAllocator(), m_DataBuffer.buffer,
                            m_DataBuffer.allocation);
                });

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "res/MovingSquare/Shaders/MovingSquare.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "res/MovingSquare/Shaders/MovingSquare.frag.spv");
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

        VkPipelineLayoutCreateInfo info = m_Engine->createPipelineLayoutCreateInfo();
        info.setLayoutCount = 1;
        info.pSetLayouts = &m_DataDescriptorSetLayout;

        m_PipelineLayout = m_Engine->setupPipelineLayout(info);

        m_Pipeline = m_Engine->setupPipeline(m_PipelineLayout);


        m_Data.model = glm::mat4(1.0f);
        m_Data.projection = glm::ortho(0.0f, 500.0f, 500.0f, 0.0f, 0.0f, -1.0f);

        void* tempBuffer;
        vmaMapMemory(*m_Engine->getAllocator(), m_DataBuffer.allocation, &tempBuffer);
            memcpy(tempBuffer, &m_Data, sizeof(Data));
        vmaUnmapMemory(*m_Engine->getAllocator(), m_DataBuffer.allocation);
    }

    void draw(VkCommandBuffer cmd) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &m_Mesh.getVertexBuffer()->buffer,
                &offset);
        vkCmdBindIndexBuffer(cmd, m_Mesh.getIndexBuffer()->buffer, 0,
                VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout,
                0, 1, &m_DataDescriptorSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, static_cast<uint32_t>(m_Mesh.getIndices()->size()),
                1, 0, 0, 0);

    }

    void update(double dt) override
    {
        posX += velX;
        posY += velY;
        updateData();
    }

    void updateData()
    {
        m_Data.model = glm::mat4(1.0f);
        m_Data.model = glm::translate(m_Data.model, glm::vec3(posX, posY, 0.0f));
        m_Data.model = glm::scale(m_Data.model, glm::vec3(100.0f));

        void* tempBuffer;
        vmaMapMemory(*m_Engine->getAllocator(), m_DataBuffer.allocation, &tempBuffer);
            memcpy(tempBuffer, &m_Data, sizeof(Data));
        vmaUnmapMemory(*m_Engine->getAllocator(), m_DataBuffer.allocation);
    }

    static bool keyboardEvent(const Eos::Events::KeyInputEvent* event)
    {
        static std::unordered_map<Eos::Events::Key, bool> activeKeys;
        Sandbox* sb = (Sandbox*)event->dataPointer;

        namespace EE = Eos::Events;

        if (event->action == EE::Action::PRESS)
        {
            activeKeys[event->key] = true;
        }
        else if (event->action == EE::Action::RELEASE)
        {
            activeKeys[event->key] = false;
        }

        sb->velX = 0.0f;
        if (activeKeys[EE::Key::KEY_A])
            sb->velX += -1.0f;
        else if (activeKeys[EE::Key::KEY_D])
            sb->velX += 1.0f;

        sb->velY = 0.0f;
        if (activeKeys[EE::Key::KEY_S])
            sb->velY += -1.0f;
        else if (activeKeys[EE::Key::KEY_W])
            sb->velY += 1.0f;

        /* if (event->key == Eos::Events::Key::KEY_A && move) */
        /*     sb->velX = -1.0f; */
        /* else if (event->key == Eos::Events::Key::KEY_D && move) */
        /*     sb->velX = 1.0f; */
        /* else */
        /*     sb->velX = 0.0f; */

        /* if (event->key == Eos::Events::Key::KEY_W && move) */
        /*     sb->velY = 1.0f; */
        /* else if (event->key == Eos::Events::Key::KEY_S && move) */
        /*     sb->velY = -1.0f; */
        /* else */
        /*     sb->velY = 0.0f; */

        return true;
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "Moving Square";

    return new Sandbox(details);
}
