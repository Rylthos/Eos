#include "DeletionQueue.hpp"

namespace Eos
{
    DeletionQueue::DeletionQueue() {}

    DeletionQueue::~DeletionQueue()
    {
        flush();
    }

    size_t DeletionQueue::pushFunction(DeleteFunction&& function)
    {
        if (m_RemovedIndices.size() == 0)
        {
            size_t index = m_Deletors.size();

            m_Deletors.push_back(function);

            return index;
        }
        else
        {
            size_t index = m_RemovedIndices.front();
            m_RemovedIndices.pop();

            m_Deletors.at(index) = function;

            return index;
        }
    }

    void DeletionQueue::removeFunction(size_t index)
    {
        m_Deletors.at(index) = [](){};

        m_RemovedIndices.push(index);
    }

    void DeletionQueue::callFunction(size_t index)
    {
        (m_Deletors.at(index))();

        removeFunction(index);
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
