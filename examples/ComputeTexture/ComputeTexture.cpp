#include "Eos/Eos.hpp"
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
    VkDescriptorSet m_ComputeSet;
    VkDescriptorSetLayout m_ComputeSetLayout;

    VkPipeline m_ComputePipeline;
    VkPipelineLayout m_ComputePipelineLayout;

    VkDescriptorSet m_RenderSet;
    VkDescriptorSetLayout m_RenderSetLayout;

    VkPipeline m_RenderPipeline;
    VkPipelineLayout m_RenderPipelineLayout;

    Eos::Texture2D m_ComputeTexture;
    Eos::Texture2D m_RenderTexture;

    Eos::IndexedMesh<Vertex, uint16_t> m_Mesh;
private:
    void windowInit() override
    {
        m_Window.setWindowSize({ 512, 512 });
        m_Window.create("Compute Texture");
    }

    void postEngineInit() override
    {
        VkExtent3D extent;
        extent.width = 512;
        extent.height = 512;
        extent.depth = 1;

        VkPipelineLayoutCreateInfo layoutInfo = Eos::Pipeline::pipelineLayoutCreateInfo();
        layoutInfo.setLayoutCount = 1;

        // Compute
        Eos::ComputeShader compShader;
        compShader.addShaderModule("res/ComputeTexture/Shaders/Main.comp.spv");

        m_ComputeTexture.createImage(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, extent,
                VMA_MEMORY_USAGE_GPU_ONLY);
        m_ComputeTexture.createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        m_ComputeTexture.addToDeletionQueue(Eos::GlobalData::getDeletionQueue());

        m_RenderTexture.createImage(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, extent,
                VMA_MEMORY_USAGE_GPU_ONLY);
        m_RenderTexture.createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        m_RenderTexture.createSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
        m_RenderTexture.addToDeletionQueue(Eos::GlobalData::getDeletionQueue());

        m_ComputeTexture.convertImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
            0, VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        m_RenderTexture.convertImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            0, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        VkDescriptorImageInfo computeImageInfo;
        computeImageInfo.imageView = m_ComputeTexture.imageView;
        computeImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        m_Engine->createDescriptorBuilder()
            .bindImage(0, &computeImageInfo, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                    VK_SHADER_STAGE_COMPUTE_BIT)
            .build(m_ComputeSet, m_ComputeSetLayout);

        layoutInfo.pSetLayouts = &m_ComputeSetLayout;

        m_Engine->createComputePipelineBuilder()
            .setShaderStage(compShader.getShaderStage())
            .build(m_ComputePipeline, m_ComputePipelineLayout, layoutInfo);

        VkCommandBuffer cmd = Eos::ComputeShader::getCommandBuffer();

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, 
                m_ComputePipelineLayout, 0, 1, &m_ComputeSet, 0, nullptr);

        uint32_t xCalls = (uint32_t)std::ceil(extent.width / 32.0f);
        uint32_t yCalls = (uint32_t)std::ceil(extent.height / 32.0f);

        vkCmdDispatch(cmd, xCalls, yCalls, 1);

        Eos::ComputeShader::endAndWait(cmd);

        Eos::ComputeShader::resetCommandBuffer(cmd);

        m_ComputeTexture.convertImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            0, VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        Eos::Texture2D::blitBetween(m_ComputeTexture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                m_RenderTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FILTER_NEAREST);

        m_RenderTexture.convertImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            0, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        // Rendering
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
                "res/ComputeTexture/Shaders/Main.vert.spv");
        shader.addShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT,
                "res/ComputeTexture/Shaders/Main.frag.spv");

        VkDescriptorImageInfo renderImageInfo;
        renderImageInfo.sampler = m_RenderTexture.sampler.value();
        renderImageInfo.imageView = m_RenderTexture.imageView;
        renderImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        m_Engine->createDescriptorBuilder()
            .bindImage(0, &renderImageInfo,
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build(m_RenderSet, m_RenderSetLayout);

        layoutInfo.pSetLayouts = &m_RenderSetLayout;

        m_Engine->createPipelineBuilder()
            .setShaderStages(shader.getShaderStages())
            .setVertexInputInfo(Vertex::getVertexDescription())
            .setViewports({ m_Window.getViewport() })
            .setScissors({ m_Window.getScissor() })
            .build(m_RenderPipeline, m_RenderPipelineLayout, layoutInfo);
    }

    void draw(VkCommandBuffer cmd) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_RenderPipeline);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &m_Mesh.getVertexBuffer()->buffer, &offset);
        vkCmdBindIndexBuffer(cmd, m_Mesh.getIndexBuffer()->buffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_RenderPipelineLayout,
                0, 1, &m_RenderSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, m_Mesh.getIndices()->size(), 1, 0, 0, 0);
    }
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details;
    details.name = "Compute Texture";

    return new Sandbox(details);
}
