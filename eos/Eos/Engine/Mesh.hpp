#pragma once

#include "Eos/EosPCH.hpp"

#include "Eos/Engine/Buffer.hpp"
#include "Eos/Core/DeletionQueue.hpp"
#include "Eos/Engine/GlobalData.hpp"
#include "Eos/Engine/ImmediateSubmit.hpp"

namespace Eos
{
    class Engine;

    template <typename T>
    concept VertexTemplate =
        requires(T t) {
            { T::getVertexDescription() } -> std::convertible_to<VertexInputDescription>;
        };

    template<VertexTemplate T>
    class Mesh
    {
    public:
        ~Mesh()
        {
            m_DeletionQueue.flush();
        }

        void setVertices(std::vector<T>& vertices) { m_Vertices = vertices; }
        std::vector<T>* getVertices() { return &m_Vertices; }

        size_t getVertexSize() const { return sizeof(T); }

        Buffer* getVertexBuffer() { return &m_VertexBuffer; }
        const Buffer* getVertexBuffer() const { return &m_VertexBuffer; }

        void setVertexBuffer(Buffer buffer) { m_VertexBuffer = buffer; }

        void update() { create(); }

        void create()
        {
            const size_t bufferSize = m_Vertices.size() * sizeof(T);

            Buffer stagingBuffer;
            stagingBuffer.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VMA_MEMORY_USAGE_CPU_ONLY);

            void* data;
            vmaMapMemory(GlobalData::getAllocator(), stagingBuffer.allocation, &data);
                memcpy(data, m_Vertices.data(), bufferSize);
            vmaUnmapMemory(GlobalData::getAllocator(), stagingBuffer.allocation);

            m_VertexBuffer.create(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

            TransferSubmit::submit([=](VkCommandBuffer cmd) {
                    VkBufferCopy copy;
                    copy.srcOffset = 0;
                    copy.dstOffset = 0;
                    copy.size = bufferSize;
                    vkCmdCopyBuffer(cmd, stagingBuffer.buffer,
                            m_VertexBuffer.buffer, 1, &copy);
                    });

            m_DeletionQueue.pushFunction([=]() {
                    vmaDestroyBuffer(GlobalData::getAllocator(), m_VertexBuffer.buffer,
                            m_VertexBuffer.allocation);
                    });

            vmaDestroyBuffer(GlobalData::getAllocator(), stagingBuffer.buffer,
                    stagingBuffer.allocation);
        }

    protected:
        std::vector<T> m_Vertices;
        Buffer m_VertexBuffer;

        DeletionQueue m_DeletionQueue;
    };

    template<VertexTemplate T, typename I>
        requires std::is_integral<I>::value
    class IndexedMesh : public Mesh<T>
    {
    public:
        void setIndices(std::vector<I>& indices) { m_Indices = indices; }
        std::vector<I>* getIndices() { return &m_Indices; }

        size_t getIndexSize() const { return sizeof(I); }

        Buffer* getIndexBuffer() { return &m_IndexBuffer; }
        const Buffer* getIndexBuffer() const { return &m_IndexBuffer; }

        void setIndexBuffer(Buffer buffer) { m_IndexBuffer = buffer; }

        void update() { create(); }

        void create()
        {
            Mesh<T>::create();

            const size_t bufferSize = m_Indices.size() * sizeof(I);

            Buffer stagingBuffer;
            stagingBuffer.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VMA_MEMORY_USAGE_CPU_ONLY);

            void* data;
            vmaMapMemory(GlobalData::getAllocator(), stagingBuffer.allocation, &data);
                memcpy(data, m_Indices.data(), bufferSize);
            vmaUnmapMemory(GlobalData::getAllocator(), stagingBuffer.allocation);

            m_IndexBuffer.create(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

            TransferSubmit::submit([=](VkCommandBuffer cmd) {
                    VkBufferCopy copy;
                    copy.srcOffset = 0;
                    copy.dstOffset = 0;
                    copy.size = bufferSize;
                    vkCmdCopyBuffer(cmd, stagingBuffer.buffer,
                            m_IndexBuffer.buffer, 1, &copy);
                    });

            Mesh<T>::m_DeletionQueue.pushFunction([=]() {
                    vmaDestroyBuffer(GlobalData::getAllocator(), m_IndexBuffer.buffer,
                            m_IndexBuffer.allocation);
                    });

            vmaDestroyBuffer(GlobalData::getAllocator(), stagingBuffer.buffer,
                    stagingBuffer.allocation);
        }

    private:
        std::vector<I> m_Indices;
        Buffer m_IndexBuffer;
    };
}

/* #include "Mesh.inl" */
