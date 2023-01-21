#include "Eos/Eos.hpp"
#include <vulkan/vulkan_core.h>

class Sandbox : public Eos::Application
{
public:
    Sandbox(const Eos::ApplicationDetails& details)
        : Eos::Application(details) {}

    ~Sandbox() {}
private:
    Eos::Buffer m_InBuffer;
    Eos::Buffer m_OutBuffer;

    VkDescriptorSet m_Set;
    VkDescriptorSetLayout m_SetLayout;

    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
private:
    void windowInit() override
    {
        m_Window.setWindowSize({ 500, 500 });
        m_Window.create("Compute Test");
    }

    void postEngineInit() override
    {
        const uint32_t elements = 10;
        const uint32_t bufferSize = elements * sizeof(int32_t);

        m_InBuffer.create(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
        m_InBuffer.addToDeletionQueue(Eos::GlobalData::getDeletionQueue());

        m_OutBuffer.create(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
        m_OutBuffer.addToDeletionQueue(Eos::GlobalData::getDeletionQueue());

        int32_t values[elements];
        for (int i = 0; i < elements; i++)
        {
            values[i] = i;
        }

        void* temp;
        vmaMapMemory(Eos::GlobalData::getAllocator(), m_InBuffer.allocation, &temp);
            memcpy(temp, &values[0], sizeof(int32_t) * elements);
        vmaUnmapMemory(Eos::GlobalData::getAllocator(), m_InBuffer.allocation);

        Eos::Shader squareShader;
        squareShader.addShaderModule(VK_SHADER_STAGE_COMPUTE_BIT,
                "res/Compute/Shaders/Square.comp.spv");

        VkDescriptorBufferInfo inBuffer{};
        inBuffer.buffer = m_InBuffer.buffer;
        inBuffer.offset = 0;
        inBuffer.range = bufferSize;

        VkDescriptorBufferInfo outBuffer{};
        outBuffer.buffer = m_OutBuffer.buffer;
        outBuffer.offset = 0;
        outBuffer.range = bufferSize;

        m_Engine->createDescriptorBuilder()
            .bindBuffer(0, &inBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_COMPUTE_BIT)
            .bindBuffer(1, &outBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    VK_SHADER_STAGE_COMPUTE_BIT)
            .build(m_Set, m_SetLayout);

        VkPipelineLayoutCreateInfo layoutInfo = Eos::Pipeline::pipelineLayoutCreateInfo();
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &m_SetLayout;

        m_Engine->createPipelineBuilder()
            .clearDefaults()
            .setShaderStages(squareShader.getShaderStages())
            .build(m_Pipeline, m_PipelineLayout, layoutInfo);

        VkCommandPoolCreateInfo commandPoolCI = Eos::Init::commandPoolCreateInfo(
                m_Engine->getComputeQueue().family, 
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        VkCommandPool commandPool;

        EOS_VK_CHECK(vkCreateCommandPool(Eos::GlobalData::getDevice(), &commandPoolCI,
                    nullptr, &commandPool));

        VkCommandBufferAllocateInfo cmdAllocInfo = Eos::Init::commandBufferAllocateInfo(
                commandPool, 1);


        VkCommandBufferBeginInfo cmdBeginInfo{};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;
        cmdBeginInfo.pInheritanceInfo = nullptr;
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VkCommandBuffer cmd;

        EOS_VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, 
                m_PipelineLayout, 0, 1, &m_Set, 0, nullptr);

        vkCmdDispatch(cmd, elements, 1, 1);

        EOS_VK_CHECK(vkEndCommandBuffer(cmd));


        vkDestroyCommandPool(Eos::GlobalData::getDevice(), commandPool, nullptr);
    }

    std::vector<VkClearValue> renderClearValues() override
    {
        return { { { { 0.1f, 0.1f, 0.1f, 1.0f } } } };
    }

    void draw(VkCommandBuffer cmd) override {}

    void update(double dt) override {}
};

Eos::Application* Eos::createApplication()
{
    ApplicationDetails details{};
    details.name = "Compute";
    details.customClearValues = true;

    return new Sandbox(details);
}
