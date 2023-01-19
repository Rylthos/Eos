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
        glm::vec3 modifiedPosition = m_Position;

        /* modifiedPosition.y *= -1; */
        /* modifiedPosition.z *= -1; */

        return glm::lookAt(modifiedPosition, modifiedPosition + m_Front, m_Up);
    }

    glm::mat4 PerspectiveCamera::getPerspectiveMatrix()
    {
        glm::mat4 perspective = glm::perspective(glm::radians(m_VFOV),
                m_WindowSize.x / m_WindowSize.y, m_NearClipping, m_FarClipping);

        return perspective;
     }

    void PerspectiveCamera::updateCameraVectors()
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        direction.y = sin(glm::radians(-m_Pitch));
        direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

        m_Front = glm::normalize(direction);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }
}
