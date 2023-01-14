#include "Eos/Eos.hpp"

struct Data
{
    glm::mat4 projection;
    glm::mat4 view;
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

    Eos::OrthographicCamera m_Camera;

    float m_PosX = 250.0f;
    float m_PosY = 250.0f;
    float m_VelX = 0.0f;
    float m_VelY = 0.0f;
    float m_VelCameraX = 0.0f;
    float m_VelCameraY = 0.0f;

    float m_MovementSpeed = 100.0f;
private:
    void init() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("Moving Square");
    }

    void postInit() override
    {
        m_Camera = Eos::OrthographicCamera(m_Window.getWindowSize());
        m_Camera.setPosition({ 100.0f, 0.0f, 0.0f });

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

        m_Mesh.create(m_Engine);

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

        VkPipelineLayoutCreateInfo info = m_Engine->createPipelineLayoutCreateInfo();
        info.setLayoutCount = 1;
        info.pSetLayouts = &m_DataDescriptorSetLayout;

        m_PipelineLayout = m_Engine->setupPipelineLayout(info);

        m_Pipeline = m_Engine->setupPipeline(m_PipelineLayout);

        m_Data.model = glm::mat4(1.0f);
        m_Data.view = glm::mat4(1.0f);
        m_Data.projection = m_Camera.getPerspectiveMatrix();

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
        m_PosX += m_VelX * m_MovementSpeed * dt;
        m_PosY += m_VelY * m_MovementSpeed * dt;
        m_Camera.getPosition().x += m_VelCameraX * m_MovementSpeed * dt;
        m_Camera.getPosition().y += m_VelCameraY * m_MovementSpeed * dt;
        updateData();
    }

    void updateData()
    {
        m_Data.model = glm::mat4(1.0f);
        m_Data.model = glm::translate(m_Data.model, glm::vec3(m_PosX, m_PosY, 0.0f));
        m_Data.model = glm::scale(m_Data.model, glm::vec3(100.0f));

        m_Data.view = m_Camera.getViewMatrix();

        void* tempBuffer;
        vmaMapMemory(*m_Engine->getAllocator(), m_DataBuffer.allocation, &tempBuffer);
            memcpy(tempBuffer, &m_Data, sizeof(Data));
        vmaUnmapMemory(*m_Engine->getAllocator(), m_DataBuffer.allocation);
    }

    static bool keyboardEvent(const Eos::Events::KeyInputEvent* event)
    {
        namespace EE = Eos::Events;

        static std::unordered_map<Eos::Events::Key, bool> activeKeys;
        Sandbox* sb = (Sandbox*)event->dataPointer;

        if (event->key == EE::Key::KEY_ESCAPE && event->action == EE::Action::PRESS)
        {
            sb->m_Window.setWindowShouldClose(true);
        }

        if (event->action == EE::Action::PRESS)
        {
            activeKeys[event->key] = true;
        }
        else if (event->action == EE::Action::RELEASE)
        {
            activeKeys[event->key] = false;
        }

        sb->m_VelX = 0.0f;
        if (activeKeys[EE::Key::KEY_A])
            sb->m_VelX += -1.0f;
        else if (activeKeys[EE::Key::KEY_D])
            sb->m_VelX += 1.0f;

        sb->m_VelY = 0.0f;
        if (activeKeys[EE::Key::KEY_S])
            sb->m_VelY += -1.0f;
        else if (activeKeys[EE::Key::KEY_W])
            sb->m_VelY += 1.0f;

        sb->m_VelCameraX = 0.0f;
        if (activeKeys[EE::Key::KEY_LEFT])
            sb->m_VelCameraX += -1.0f;
        else if (activeKeys[EE::Key::KEY_RIGHT])
            sb->m_VelCameraX += 1.0f;

        sb->m_VelCameraY = 0.0f;
        if (activeKeys[EE::Key::KEY_DOWN])
            sb->m_VelCameraY += -1.0f;
        else if (activeKeys[EE::Key::KEY_UP])
            sb->m_VelCameraY += 1.0f;

        return true;
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "Moving Square";

    return new Sandbox(details);
}
