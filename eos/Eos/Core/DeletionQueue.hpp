#pragma once

#include "Eos/EosPCH.hpp"

#include <functional>
#include <queue>

namespace Eos
{
    using DeleteFunction = std::function<void()>;

    class EOS_API DeletionQueue
    {
    public:
        DeletionQueue();
        DeletionQueue(const DeletionQueue&) = delete;
        DeletionQueue(const DeletionQueue&&) = delete;
        ~DeletionQueue();

        size_t pushFunction(DeleteFunction&& function);
        void removeFunction(size_t index);
        void callFunction(size_t index);

        void flush();
    private:
        std::queue<size_t> m_RemovedIndices;
        std::vector<DeleteFunction> m_Deletors;
    };
}
