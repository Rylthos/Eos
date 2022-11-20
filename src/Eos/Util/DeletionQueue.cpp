#include "DeletionQueue.hpp"

namespace Eos
{
    DeletionQueue::DeletionQueue() {}

    DeletionQueue::~DeletionQueue()
    {
        flush();
    }

    void DeletionQueue::pushFunction(DeleteFunction&& function)
    {
        m_Deletors.push_back(function);
    }

    void DeletionQueue::flush()
    {
        for (auto it = m_Deletors.rbegin(); it != m_Deletors.rend(); it++)
        {
            (*it)();
        }

        m_Deletors.clear();
    }
}
