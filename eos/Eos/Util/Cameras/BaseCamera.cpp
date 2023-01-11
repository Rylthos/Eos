#include "BaseCamera.hpp"

namespace Eos
{
    BaseCamera::BaseCamera()
        : m_WindowSize(0.0f), m_Position(0.0f) { }

    BaseCamera::BaseCamera(glm::vec2 windowSize)
        : m_WindowSize(windowSize), m_Position(0.0f) { }
}
