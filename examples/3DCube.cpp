#include "Eos/Eos.hpp"
#include <vulkan/vulkan_core.h>

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

    float m_PreviousMouseX = -1.0f;
    float m_PreviousMouseY = -1.0f;
    bool m_CapturedMouse = false;
    bool m_RightClick = false;

private:
    void windowInit() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("3D Cube");
    }

    void renderPassInit(Eos::RenderPass& renderPass) override
    {
        VkAttachmentDescription colourAttachment{};
        colourAttachment.format = m_Engine->getSwapchain().imageFormat;
        colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkSubpassDependency colourDependency{};
        colourDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        colourDependency.dstSubpass = 0;
        colourDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        colourDependency.srcAccessMask = 0;
        colourDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        colourDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        Eos::RenderPassBuilder::begin(renderPass)
            .addAttachment(colourAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    colourDependency)
            .addDefaultDepthBuffer(m_Window.getWindowSize().x, m_Window.getWindowSize().y)
            .build();
    }

    std::vector<VkImageView> framebufferCreation(VkFramebufferCreateInfo& framebuffer,
            VkImageView& swapchainImage, Eos::RenderPass& renderpass) override
    {
        std::vector<VkImageView> attachments = 
        { swapchainImage, renderpass.depthImage->imageView };

        return attachments;
    }

    void postEngineInit() override
    {
        m_MainEventDispatcher.addCallback(&keyboardEvent, this);
        m_MainEventDispatcher.addCallback(&mouseMoveEvent, this);
        m_MainEventDispatcher.addCallback(&mousePressEvent, this);

        m_Camera = Eos::PerspectiveCamera(m_Window.getWindowSize());
        m_Camera.setNearClippingPlane(0.1f);
        m_Camera.setFarClippingPlane(200.0f);

        m_Camera.getPosition().y -= 0.0f;
        /* m_Camera.getPosition().z += 3.0f; */

        m_Camera.setYaw(90.0f);
        m_Camera.setPitch(0.0f);

        std::vector<Vertex> vertices = {
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }}, // 0
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }}, // 1
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }}, // 2
            { {  1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }}, // 3
            { { -1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f }}, // 4
            { {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f }}, // 5
            { { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }}, // 6
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }}, // 7
        };

        std::vector<uint16_t> indices = {
            // Front
            0, 1, 2,
            1, 2, 3,

            // Back
            4, 5, 6,
            5, 6, 7,

            // Right
            1, 3, 5,
            3, 5, 7,

            // Left
            0, 2, 4,
            2, 4, 6,

            // Up
            0, 1, 4,
            1, 4, 5,

            // Down
            2, 3, 6,
            3, 6, 7,
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

    std::vector<VkClearValue> renderClearValues() override
    {
        VkClearValue background = { { { 0.1f, 0.1f, 0.1f, 1.0f } } };

        VkClearValue depthColour{};
        depthColour.depthStencil.depth = 1.0f;

        return { background, depthColour };
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

        glm::vec3 front = m_Camera.getFrontVector() * movementAmount;
        glm::vec3 right = m_Camera.getRightVector() * movementAmount;
        glm::vec3 up = m_Camera.getUpVector() * movementAmount;

        // Up / Down
        if (m_ActiveKeys[Eos::Events::Key::KEY_SPACE])
            m_Camera.getPosition() -= up;
        else if (m_ActiveKeys[Eos::Events::Key::KEY_LEFT_CONTROL])
            m_Camera.getPosition() += up;

        // Left / Right
        if (m_ActiveKeys[Eos::Events::Key::KEY_A])
            m_Camera.getPosition() -= right;
        else if (m_ActiveKeys[Eos::Events::Key::KEY_D])
            m_Camera.getPosition() += right;

        // Forward / Back
        if (m_ActiveKeys[Eos::Events::Key::KEY_W])
            m_Camera.getPosition() += front;
        else if (m_ActiveKeys[Eos::Events::Key::KEY_S])
            m_Camera.getPosition() -= front;

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

    static bool mouseMoveEvent(const Eos::Events::MouseMoveEvent* event)
    {
        Sandbox* sb = (Sandbox*)event->dataPointer;

        if (!sb->m_RightClick)
            return false;

        if (!sb->m_CapturedMouse)
        {
            sb->m_PreviousMouseX = event->xPos;
            sb->m_PreviousMouseY = event->yPos;

            sb->m_CapturedMouse = true;
        }

        const float mouseSens = 0.5f;

        float offsetX = (sb->m_PreviousMouseX - event->xPos) * mouseSens;
        float offsetY = (sb->m_PreviousMouseY - event->yPos) * mouseSens;

        sb->m_Camera.getPitch() += offsetY;
        sb->m_Camera.getYaw() += offsetX;

        sb->m_PreviousMouseX = event->xPos;
        sb->m_PreviousMouseY = event->yPos;

        sb->updateData();

        return true;
    }

    static bool mousePressEvent(const Eos::Events::MousePressEvent* event)
    {
        Sandbox* sb = (Sandbox*)event->dataPointer;

        if (event->button == Eos::Events::MouseButton::MOUSE_BUTTON_RIGHT)
        {
            if (event->action == Eos::Events::Action::PRESS)
            {
                sb->m_RightClick = true;
                sb->m_Window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                sb->m_CapturedMouse = false;
            }
            else
            {
                sb->m_RightClick = false;
                sb->m_Window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }

        return true;
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "3D Cube";
    details.enableVsync = false;
    details.customRenderpass = true;
    details.customClearValues = true;

    return new Sandbox(details);
}
