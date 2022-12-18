#pragma once

#include "../Util/Types.hpp"
#include <vector>
#include <concepts>

namespace Eos
{
    template <typename T>
    concept VertexDescription =
        requires(T t) {
            { T::getVertexDescription() } -> std::convertible_to<VertexInputDescription>;
        };

    template<VertexDescription T>
    class Mesh
    {
    public:
        void setVertices(std::vector<T>& vertices) { m_Vertices = vertices; }
        std::vector<T>* getVertices() { return &m_Vertices; }

        size_t getVertexSize() const { return sizeof(T); }

        Buffer* getVertexBuffer() { return &m_VertexBuffer; }
        const Buffer* getVertexBuffer() const { return &m_VertexBuffer; }

        void setVertexBuffer(Buffer buffer) { m_VertexBuffer = buffer; }
    protected:
        std::vector<T> m_Vertices;
        Buffer m_VertexBuffer;
    };

    template<VertexDescription T, typename I>
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
    protected:
        std::vector<I> m_Indices;
        Buffer m_IndexBuffer;
    };
}