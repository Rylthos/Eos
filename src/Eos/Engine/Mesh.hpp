#pragma once

#include "../Util/Types.hpp"
#include <vector>

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

        Buffer* getBuffer() { return &m_VertexBuffer; }
        const Buffer* getBuffer() const { return &m_VertexBuffer; }
    private:
        std::vector<T> m_Vertices;
        Buffer m_VertexBuffer;
    };
}
