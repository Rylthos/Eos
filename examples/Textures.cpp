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
    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;

    VkDescriptorSet m_Set;
    VkDescriptorSetLayout m_SetLayout;

    Eos::Texture2D m_Texture;

    Eos::IndexedMesh<Vertex, uint16_t> m_Mesh;
private:
    void windowInit() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("Texture");
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

        // Create Texture
        VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
        size_t imageSize = 500 * 500 * 4;
        VkExtent3D imageExtent;
        imageExtent.width = 500;
        imageExtent.height = 500;
        imageExtent.depth = 1;

        uint8_t pixels[500 * 500 * 4];
        memset(pixels, 125, 500 * 500 * 4 * sizeof(uint8_t));

        Eos::Buffer stagingBuffer;
        stagingBuffer.create(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VMA_MEMORY_USAGE_CPU_ONLY);

        m_Texture.createImage(imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                imageExtent, VMA_MEMORY_USAGE_GPU_ONLY);
        m_Texture.createImageView(imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        m_Texture.addToDeletionQueue(Eos::GlobalData::getDeletionQueue());

        void* data;
        vmaMapMemory(Eos::GlobalData::getAllocator(), stagingBuffer.allocation, &data);
            memcpy(data, pixels, imageSize);
        vmaUnmapMemory(Eos::GlobalData::getAllocator(), stagingBuffer.allocation);

        Eos::TransferSubmit::submit([&](VkCommandBuffer cmd) {
            VkImageSubresourceRange range;
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            VkImageMemoryBarrier imageBarrierToTransfer{};
            imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageBarrierToTransfer.image = m_Texture.image;
            imageBarrierToTransfer.subresourceRange = range;
            imageBarrierToTransfer.srcAccessMask = 0;
            imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
                    0, nullptr, 1, &imageBarrierToTransfer);

            VkBufferImageCopy copyRegion{};
            copyRegion.bufferOffset = 0;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.mipLevel = 0;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageExtent = imageExtent;

            vkCmdCopyBufferToImage(cmd, stagingBuffer.buffer, m_Texture.image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            VkImageMemoryBarrier imageBarrierToReadable = imageBarrierToTransfer;
            imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr,
                    1, &imageBarrierToReadable);
        });


        vmaDestroyBuffer(Eos::GlobalData::getAllocator(), stagingBuffer.buffer,
                stagingBuffer.allocation);

        VkSamplerCreateInfo samplerCI{};
        samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCI.pNext = nullptr;
        samplerCI.magFilter = VK_FILTER_NEAREST;
        samplerCI.minFilter = VK_FILTER_NEAREST;
        samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        VkSampler sampler;
        vkCreateSampler(Eos::GlobalData::getDevice(), &samplerCI, nullptr, &sampler);

        VkDescriptorImageInfo imageBI;
        imageBI.sampler = sampler;
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
            .setViewports({ m_Window.getViewport() })
            .setScissors({ m_Window.getScissor() })
            .build(m_Pipeline, m_PipelineLayout, layoutInfo);

        Eos::GlobalData::getDeletionQueue().pushFunction([=](){
            vkDestroySampler(Eos::GlobalData::getDevice(), sampler, nullptr);
        });
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
