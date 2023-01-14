#include "Eos/Eos.hpp"

#include <queue>

struct GlobalShaderData
{
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

struct SegmentShaderData
{
    glm::mat4 modelMatrix;
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

struct SnakeSegment
{
    glm::ivec2 position;
};

struct AppleSegment
{
    glm::ivec2 position;
};

enum class SnakeDirection
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

class Snake : public Eos::Application
{
public:
    Snake(const Eos::ApplicationDetails& details)
        : Eos::Application(details) {}

    ~Snake() {}
private:
    VkPipeline m_SnakePipeline;
    VkPipelineLayout m_SnakePipelineLayout;

    VkPipeline m_ApplePipeline;
    VkPipelineLayout m_ApplePipelineLayout;

    uint16_t m_CurrentSegment = 0;

    Eos::IndexedMesh<Vertex, uint16_t> m_GeneralMesh;

    VkDescriptorSet m_SnakeSet;
    VkDescriptorSetLayout m_SnakeSetLayout;

    VkDescriptorSet m_AppleSet;
    VkDescriptorSetLayout m_AppleSetLayout;

    Eos::Buffer m_GlobalDataBuffer;
    Eos::Buffer m_SegmentDataBuffer;

    Eos::OrthographicCamera m_Camera;

    const uint16_t m_Cols = 10;
    const uint16_t m_Rows = 10;

    uint16_t m_CellWidth;
    uint16_t m_CellHeight;

    const uint16_t m_MaxSegments = m_Cols * m_Rows;

    const uint16_t m_AppleCount = 1;

    std::vector<SnakeSegment> m_Snake;
    std::vector<AppleSegment> m_Apples;

    std::queue<SnakeDirection> m_MoveQueue;

    float m_UpdateFPS = 10.0f;
    int16_t m_VelX = 1;
    int16_t m_VelY = 0;
private:
    void init() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("Snake");
    }

    void postInit() override
    {
        m_Camera = Eos::OrthographicCamera(m_Window.getWindowSize());

        m_MainEventDispatcher.addCallback(&keyboardEvent, this);

        std::vector<Vertex> vertices = {
            { { -1.0f,  1.0f, 0.0f} },
            { {  1.0f,  1.0f, 0.0f} },
            { { -1.0f, -1.0f, 0.0f} },
            { {  1.0f, -1.0f, 0.0f} },
        };
        m_GeneralMesh.setVertices(vertices);

        std::vector<uint16_t> indices = {
            0, 1, 2,
            1, 2, 3
        };
        m_GeneralMesh.setIndices(indices);

        m_GeneralMesh.create(m_Engine);

        m_Snake.reserve(m_MaxSegments);
        m_Apples.reserve(m_AppleCount);

        m_CellWidth = m_Window.getWindowSize().x / m_Cols;
        m_CellHeight = m_Window.getWindowSize().y / m_Rows;

        reset();

        createPipelines();

        updateGlobalData();
        updateSegmentData();
    }

    void draw(VkCommandBuffer cmd) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_SnakePipeline);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &m_GeneralMesh.getVertexBuffer()->buffer,
                &offset);
        vkCmdBindIndexBuffer(cmd, m_GeneralMesh.getIndexBuffer()->buffer,
                0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_SnakePipelineLayout,
                0, 1, &m_SnakeSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, 6, m_Snake.size(), 0, 0, 0);


        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ApplePipeline);

        vkCmdBindVertexBuffers(cmd, 0, 1, &m_GeneralMesh.getVertexBuffer()->buffer,
                &offset);
        vkCmdBindIndexBuffer(cmd, m_GeneralMesh.getIndexBuffer()->buffer,
                0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ApplePipelineLayout,
                0, 1, &m_AppleSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, 6, m_Apples.size(), 0, 0, 0);
    }

    void update(double dt) override
    {
        static float dtTotal = 0.0f;

        dtTotal += dt;
        if (dtTotal >= (1.0f / m_UpdateFPS))
        {
            dtTotal = 0;

            for (int i = m_Snake.size() - 1; i >= 1; i--)
            {
                m_Snake[i] = m_Snake[i - 1];
            }

            m_Snake[0].position.x += m_VelX;
            m_Snake[0].position.y += m_VelY;

            if (m_MoveQueue.size() != 0)
            {
                SnakeDirection dir = m_MoveQueue.front();
                m_MoveQueue.pop();

                switch (dir)
                {
                    case SnakeDirection::LEFT:
                        if (m_VelX == 1) break;
                        m_VelX = -1;
                        m_VelY = 0;
                        break;
                    case SnakeDirection::RIGHT:
                        if (m_VelX == -1) break;
                        m_VelX = 1;
                        m_VelY = 0;
                        break;
                    case SnakeDirection::UP:
                        if (m_VelY == 1) break;
                        m_VelX = 0;
                        m_VelY = -1;
                        break;
                    case SnakeDirection::DOWN:
                        if (m_VelY == -1) break;
                        m_VelX = 0;
                        m_VelY = 1;
                        break;
                }
            }
            checkApples();

            checkEnd();

            updateSegmentData();
        }
    }

    void createPipelines()
    {
        m_Engine->getPipelineBuilder()->addVertexInputInfo(Vertex::getVertexDescription());

        Eos::Shader shader;
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "res/Snake/Shaders/Snake.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "res/Snake/Shaders/Snake.frag.spv");
        m_Engine->getPipelineBuilder()->shaderStages = shader.getShaderStages();

        m_GlobalDataBuffer = m_Engine->createBuffer(sizeof(GlobalShaderData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VMA_MEMORY_USAGE_CPU_TO_GPU);

        m_SegmentDataBuffer = m_Engine->createBuffer(sizeof(SegmentShaderData) * (m_MaxSegments + m_AppleCount),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

        m_Engine->getDeletionQueue()->pushFunction([&]()
                {
                    vmaDestroyBuffer(*m_Engine->getAllocator(), m_GlobalDataBuffer.buffer,
                            m_GlobalDataBuffer.allocation);

                    vmaDestroyBuffer(*m_Engine->getAllocator(), m_SegmentDataBuffer.buffer,
                            m_SegmentDataBuffer.allocation);
                });

        VkDescriptorBufferInfo globalInfo{};
        globalInfo.buffer = m_GlobalDataBuffer.buffer;
        globalInfo.offset = 0;
        globalInfo.range = sizeof(GlobalShaderData);

        VkDescriptorBufferInfo segmentInfo{};
        segmentInfo.buffer = m_SegmentDataBuffer.buffer;
        segmentInfo.offset = 0;
        segmentInfo.range = sizeof(SegmentShaderData) * m_MaxSegments;

        m_Engine->createDescriptorBuilder()
            .bindBuffer(0, &globalInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT)
            .bindBuffer(1, &segmentInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT)
            .build(m_SnakeSet, m_SnakeSetLayout);

        VkPipelineLayoutCreateInfo info = m_Engine->createPipelineLayoutCreateInfo();
        info.setLayoutCount = 1;
        info.pSetLayouts = &m_SnakeSetLayout;

        m_SnakePipelineLayout = m_Engine->setupPipelineLayout(info);
        m_SnakePipeline = m_Engine->setupPipeline(m_SnakePipelineLayout);

        shader.clearModules();
        shader.addShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "res/Snake/Shaders/Snake.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "res/Snake/Shaders/Apple.frag.spv");
        m_Engine->getPipelineBuilder()->shaderStages = shader.getShaderStages();

        segmentInfo.buffer = m_SegmentDataBuffer.buffer;
        segmentInfo.offset = sizeof(SegmentShaderData) * m_MaxSegments;
        segmentInfo.range = sizeof(SegmentShaderData) * m_AppleCount;

        m_Engine->createDescriptorBuilder()
            .bindBuffer(0, &globalInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT)
            .bindBuffer(1, &segmentInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_VERTEX_BIT)
            .build(m_AppleSet, m_AppleSetLayout);

        info.setLayoutCount = 1;
        info.pSetLayouts = &m_AppleSetLayout;

        m_ApplePipelineLayout = m_Engine->setupPipelineLayout(info);
        m_ApplePipeline = m_Engine->setupPipeline(m_ApplePipelineLayout);
    }

    void reset()
    {
        m_Snake.clear();
        m_Apples.clear();

        m_VelX = 1.0f;
        m_VelY = 0.0f;

        int currentCol = m_Cols / 2;
        int currentRow = m_Rows / 2;
        for (int i = 0; i < 3; i++)
        {
            m_Snake.push_back({ { currentCol, currentRow } });
            currentCol--;
        }

        for (int i = 0; i < m_AppleCount; i++)
        {
            m_Apples.push_back({ { 0, 0 } });
            resetApplePosition(i);
        }
    }

    void checkEnd()
    {
        SnakeSegment headSegment = m_Snake[0];

        if (m_Snake.size() == m_MaxSegments)
        {
            reset();
            return;
        }

        if (m_Snake.size() >= 5)
        {
            for (int i = 4; i < m_Snake.size(); i++)
            {
                SnakeSegment currentSegment = m_Snake[i];
                if (headSegment.position.x == currentSegment.position.x &&
                        headSegment.position.y == currentSegment.position.y)
                {
                    reset();
                    return;
                }
            }
        }

        if (headSegment.position.x == -1 || headSegment.position.x == m_Cols
                || headSegment.position.y == -1 || headSegment.position.y == m_Rows)
        {
            reset();
            return;
        }
    }

    void checkApples()
    {
        for (int i = 0; i < m_AppleCount; i++)
        {
            if (checkCollision(m_Apples[i].position.x, m_Apples[i].position.y))
            {
                resetApplePosition(i);
                increaseSnakeLength();
            }
        }
    }

    void resetApplePosition(int index = 0)
    {
        int x, y;
        do
        {
            x = rand() % m_Cols;
            y = rand() % m_Rows;
        }
        while(checkCollision(x, y));

        m_Apples[index].position = { x, y };
    }

    bool checkCollision(int x, int y)
    {
        for (int i = 0; i < m_Snake.size(); i++)
        {
            if (m_Snake[i].position.x == x && m_Snake[i].position.y == y)
            {
                return true;
            }
        }

        if (m_AppleCount > 1)
        {
            int amount = 0;
            for (int i = 0; i < m_Apples.size(); i++)
            {
                if (m_Apples[i].position.x == x && m_Apples[i].position.y == y)
                {
                    amount++;

                    if (amount == 2)
                        return true;
                }
            }
        }

        return false;
    }

    void increaseSnakeLength()
    {
        m_Snake.push_back({ { 0, 0 } });
        m_Snake[m_Snake.size() - 1] = m_Snake[m_Snake.size() - 2];

        updateSegmentData();
    }

    void updateGlobalData()
    {
        GlobalShaderData data;
        data.projectionMatrix = m_Camera.getPerspectiveMatrix();
        data.viewMatrix = m_Camera.getViewMatrix();

        void* temp;
        vmaMapMemory(*m_Engine->getAllocator(), m_GlobalDataBuffer.allocation, &temp);
            memcpy(temp, &data, sizeof(GlobalShaderData));
        vmaUnmapMemory(*m_Engine->getAllocator(), m_GlobalDataBuffer.allocation);
    }
    
    void updateSegmentData()
    {
        SegmentShaderData data[m_MaxSegments + m_AppleCount];

        glm::mat4 model;
        for (int i = 0; i < m_Snake.size(); i++)
        {
            SnakeSegment segment = m_Snake[i];
            glm::vec2 position = gridPositionToWorldPos(segment.position);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(position.x, position.y, 0));
            model = glm::scale(model, glm::vec3(m_CellWidth / 2, m_CellHeight / 2, 1.0f));
            data[i].modelMatrix = model;
        }

        for (int i = 0; i < m_Apples.size(); i++)
        {
            AppleSegment segment = m_Apples[i];
            glm::vec2 position = gridPositionToWorldPos(segment.position);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(position.x, position.y, 0));
            model = glm::scale(model, glm::vec3(m_CellWidth / 2, m_CellHeight / 2, 1.0f));

            data[m_MaxSegments + i].modelMatrix = model;
        }

        void* temp;
        vmaMapMemory(*m_Engine->getAllocator(), m_SegmentDataBuffer.allocation, &temp);
            memcpy(temp, &data, sizeof(SegmentShaderData) * (m_MaxSegments + m_AppleCount));
        vmaUnmapMemory(*m_Engine->getAllocator(), m_SegmentDataBuffer.allocation);
    }

    glm::vec2 gridPositionToWorldPos(glm::ivec2 pos)
    {
        glm::vec2 position;
        position.x = pos.x * m_CellWidth + (m_CellWidth / 2);
        position.y = pos.y * m_CellHeight + (m_CellHeight / 2);

        return position;
    }

    static bool keyboardEvent(const Eos::Events::KeyInputEvent* event)
    {
        Snake* snake = (Snake*)event->dataPointer;

        if (event->action == Eos::Events::Action::PRESS)
        {
            if (event->key == Eos::Events::Key::KEY_ESCAPE)
                snake->m_Window.setWindowShouldClose(true);

            if (event->key == Eos::Events::Key::KEY_A)
                snake->m_MoveQueue.push(SnakeDirection::LEFT);

            if (event->key == Eos::Events::Key::KEY_D)
                snake->m_MoveQueue.push(SnakeDirection::RIGHT);

            if (event->key == Eos::Events::Key::KEY_W)
                snake->m_MoveQueue.push(SnakeDirection::UP);

            if (event->key == Eos::Events::Key::KEY_S)
                snake->m_MoveQueue.push(SnakeDirection::DOWN);

            if (event->key == Eos::Events::Key::KEY_SPACE)
                snake->increaseSnakeLength();
        }

        return true;
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "Snake";

    return new Snake(details);
}
