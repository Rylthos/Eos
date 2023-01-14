#include "Mesh.hpp"

#include "vk_mem_alloc.h"

namespace Eos
{
    template <VertexTemplate T>
    void Mesh<T>::create(Engine* engine)
    {
        const size_t bufferSize = m_Vertices.size() * sizeof(T);

        Buffer stagingBuffer = engine->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VMA_MEMORY_USAGE_CPU_ONLY);

        void* data;
        vmaMapMemory(*engine->getAllocator(), stagingBuffer.allocation, &data);
            memcpy(data, m_Vertices.data(), bufferSize);
        vmaUnmapMemory(*engine->getAllocator(), stagingBuffer.allocation);

        setVertexBuffer(engine->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY));

        engine->immediateSubmit([=](VkCommandBuffer cmd) {
                VkBufferCopy copy;
                copy.srcOffset = 0;
                copy.dstOffset = 0;
                copy.size = bufferSize;
                vkCmdCopyBuffer(cmd, stagingBuffer.buffer,
                        m_VertexBuffer.buffer, 1, &copy);
                });

        engine->getDeletionQueue()->pushFunction([=]() {
                vmaDestroyBuffer(*engine->getAllocator(), m_VertexBuffer.buffer,
                        m_VertexBuffer.allocation);
                });

        vmaDestroyBuffer(*engine->getAllocator(), stagingBuffer.buffer,
                stagingBuffer.allocation);
    }

    template<VertexTemplate T, typename I>
        requires std::is_integral<I>::value
    void IndexedMesh<T,I>::create(Engine* engine)
    {
        Mesh<T>::create(engine);

        const size_t bufferSize = m_Indices.size() * sizeof(I);

        Buffer stagingBuffer = engine->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VMA_MEMORY_USAGE_CPU_ONLY);

        void* data;
        vmaMapMemory(*engine->getAllocator(), stagingBuffer.allocation, &data);
            memcpy(data, m_Indices.data(), bufferSize);
        vmaUnmapMemory(*engine->getAllocator(), stagingBuffer.allocation);

        setIndexBuffer(engine->createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY));

        engine->immediateSubmit([=](VkCommandBuffer cmd) {
                VkBufferCopy copy;
                copy.srcOffset = 0;
                copy.dstOffset = 0;
                copy.size = bufferSize;
                vkCmdCopyBuffer(cmd, stagingBuffer.buffer,
                        m_IndexBuffer.buffer, 1, &copy);
                });

        engine->getDeletionQueue()->pushFunction([=]() {
                vmaDestroyBuffer(*engine->getAllocator(), m_IndexBuffer.buffer,
                m_IndexBuffer.allocation);
        });
        vmaDestroyBuffer(*engine->getAllocator(), stagingBuffer.buffer,
                stagingBuffer.allocation);
    }
}
