#pragma once

#include <functional>
#include <deque>

namespace Eos
{
    using DeleteFunction = std::function<void()>;

    class DeletionQueue
    {
    public:
        DeletionQueue();
        DeletionQueue(const DeletionQueue&) = delete;
        DeletionQueue(const DeletionQueue&&) = delete;
        ~DeletionQueue();

        void pushFunction(DeleteFunction&& function);
        void flush();
    private:
        std::deque<DeleteFunction> m_Deletors;
    };
}
