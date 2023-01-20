#include "Eos/Eos.hpp"

struct ModelData
{
    glm::mat4 perspectiveMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
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
    Sandbox(const Eos::ApplicationDetails& details)
        : Eos::Application(details) {}

    ~Sandbox() {}
private:
    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;

    VkDescriptorSet m_CubeSet;
    VkDescriptorSetLayout m_CubeSetLayout;

    Eos::Buffer m_CubeDataBuffer;

    Eos::IndexedMesh<Vertex, uint16_t> m_CubeMesh;

    Eos::PerspectiveCamera m_Camera;

    std::unordered_map<Eos::Events::Key, bool> m_ActiveKeys;

private:
    void init() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("3D Cube");
    }

    void postInit() override
    {
        m_MainEventDispatcher.addCallback(&keyboardEvent, this);

        m_Camera = Eos::PerspectiveCamera(m_Window.getWindowSize());
        m_Camera.setNearClippingPlane(0.1f);
        m_Camera.setFarClippingPlane(200.0f);

        m_Camera.getPosition().y -= 0.0f;
        /* m_Camera.getPosition().z += 3.0f; */

        m_Camera.setYaw(90.0f);
        m_Camera.setPitch(0.0f);

        std::vector<Vertex> vertices = {
            // Front
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f } }, // 0
            { {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f } }, // 1
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f } }, // 2
            { {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f } }, // 3

            // Back
            { { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } }, // 4
            { {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } }, // 5
            { { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } }, // 6
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } }, // 7

            // Left
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } }, // 8
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } }, // 9
            { { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f } }, // 10
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f } }, // 11

            // Right
            { {  1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f } }, // 12
            { {  1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f } }, // 13
            { {  1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f } }, // 14
            { {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f } }, // 15

            // Up
            { { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f } }, // 16
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f } }, // 17
            { { -1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f } }, // 18
            { {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f } }, // 19

            // Down
            { { -1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f } }, // 20
            { {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f } }, // 21
            { { -1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f } }, // 22
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f } }, // 23
        };

        std::vector<uint16_t> indices = {
            // Front
            0, 1, 2,
            1, 2, 3,

            // Back
            4, 5, 6,
            5, 6, 7,

            // Left
            8, 9, 10,
            9, 10, 11,

            // Right
            12, 13, 14,
            13, 14, 15,

            // Up
            16, 17, 18,
            17, 18, 19,

            // Down
            20, 21, 22,
            21, 22, 23,
        };

        m_CubeMesh.setVertices(vertices);
        m_CubeMesh.setIndices(indices);

        m_CubeMesh.create();

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "res/3DCube/Shaders/3DCube.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "res/3DCube/Shaders/3DCube.frag.spv");

        m_CubeDataBuffer.create(sizeof(ModelData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VMA_MEMORY_USAGE_CPU_TO_GPU);
        m_CubeDataBuffer.addToDeletionQueue(Eos::GlobalData::getDeletionQueue());

        VkDescriptorBufferInfo modelInfo{};
        modelInfo.buffer = m_CubeDataBuffer.buffer;
        modelInfo.offset = 0;
        modelInfo.range = sizeof(ModelData);

        VkPipelineLayoutCreateInfo layoutInfo = Eos::Pipeline::pipelineLayoutCreateInfo();
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &m_CubeSetLayout;

        m_Engine->createDescriptorBuilder()
            .bindBuffer(0, &modelInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT)
            .build(m_CubeSet, m_CubeSetLayout);

        m_Engine->createPipelineBuilder()
            .setShaderStages(shader.getShaderStages())
            .setVertexInputInfo(Vertex::getVertexDescription())
            .setViewports({ m_Window.getViewport() })
            .setScissors({ m_Window.getScissor() })
            .build(m_Pipeline, m_PipelineLayout, layoutInfo);

        ModelData modelData;
        modelData.perspectiveMatrix = m_Camera.getPerspectiveMatrix();
        modelData.viewMatrix = m_Camera.getViewMatrix();

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f));
        modelData.modelMatrix = model;

        void* temp;
        vmaMapMemory(Eos::GlobalData::getAllocator(), m_CubeDataBuffer.allocation, &temp);
            memcpy(temp, &modelData, sizeof(ModelData));
        vmaUnmapMemory(Eos::GlobalData::getAllocator(), m_CubeDataBuffer.allocation);
    }

    void draw(VkCommandBuffer cmd) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &m_CubeMesh.getVertexBuffer()->buffer,
                &offset);
        vkCmdBindIndexBuffer(cmd, m_CubeMesh.getIndexBuffer()->buffer,
                0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout,
                0, 1, &m_CubeSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, m_CubeMesh.getIndices()->size(), 1, 0, 0, 0);
    }

    void update(double dt) override
    {
        float movementAmount = 5.0f * dt;
        // Up / Down
        if (m_ActiveKeys[Eos::Events::Key::KEY_SPACE])
            m_Camera.getPosition().y -= movementAmount;
        else if (m_ActiveKeys[Eos::Events::Key::KEY_LEFT_CONTROL])
            m_Camera.getPosition().y += movementAmount;

        // Left / Right
        if (m_ActiveKeys[Eos::Events::Key::KEY_A])
            m_Camera.getPosition().x -= movementAmount;
        else if (m_ActiveKeys[Eos::Events::Key::KEY_D])
            m_Camera.getPosition().x += movementAmount;

        updateData();
    }

    void updateData()
    {
        ModelData modelData;
        modelData.perspectiveMatrix = m_Camera.getPerspectiveMatrix();
        modelData.viewMatrix = m_Camera.getViewMatrix();

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f));
        modelData.modelMatrix = model;

        void* temp;
        vmaMapMemory(Eos::GlobalData::getAllocator(), m_CubeDataBuffer.allocation, &temp);
            memcpy(temp, &modelData, sizeof(ModelData));
        vmaUnmapMemory(Eos::GlobalData::getAllocator(), m_CubeDataBuffer.allocation);
    }

    static bool keyboardEvent(const Eos::Events::KeyInputEvent* event)
    {
        Sandbox* sb = (Sandbox*)event->dataPointer;

        if (event->action == Eos::Events::Action::PRESS)
            sb->m_ActiveKeys[event->key] = true;
        else if (event->action == Eos::Events::Action::RELEASE)
            sb->m_ActiveKeys[event->key] = false;

        if (sb->m_ActiveKeys[Eos::Events::Key::KEY_ESCAPE])
            sb->m_Window.setWindowShouldClose(true);

        return true;
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "3D Cube";

    return new Sandbox(details);
}
