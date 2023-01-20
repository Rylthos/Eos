#include "Perspective.hpp"

namespace Eos
{
    PerspectiveCamera::PerspectiveCamera()
        : BaseCamera() { updateCameraVectors(); }

    PerspectiveCamera::PerspectiveCamera(glm::vec2 windowSize)
        : BaseCamera(windowSize) { updateCameraVectors(); }

    PerspectiveCamera::~PerspectiveCamera() {}

    glm::mat4 PerspectiveCamera::getViewMatrix()
    {
        if (m_Changed) updateCameraVectors();

        return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    glm::mat4 PerspectiveCamera::getPerspectiveMatrix()
    {
        if (m_Changed) updateCameraVectors();

        glm::mat4 perspective = glm::perspective(glm::radians(m_VFOV),
                m_WindowSize.x / m_WindowSize.y, m_NearClipping, m_FarClipping);

        return perspective;
     }

    void PerspectiveCamera::updateCameraVectors()
    {
        m_Pitch = std::clamp(m_Pitch, -89.9f, 89.9f);
        glm::vec3 direction;
        direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        direction.y = sin(glm::radians(-m_Pitch));
        direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

        m_Front = glm::normalize(direction);
        // Flipped due to "World Up" being the opposite
        m_Right = -glm::normalize(glm::cross(m_Front, m_WorldUp));
        // Fliped due to the "Right Vector" pointing the wrong way for the "Up Vector"
        m_Up = -glm::normalize(glm::cross(m_Right, m_Front));

        m_Changed = false;
    }
}
