#pragma once

#include "Eos/EosPCH.hpp"
#include "Eos/Core/Cameras/Base.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace Eos
{
    class EOS_API PerspectiveCamera : public BaseCamera
    {
    public:
        PerspectiveCamera();
        PerspectiveCamera(glm::vec2 windowSize);
        PerspectiveCamera(PerspectiveCamera& camera) = default;

        ~PerspectiveCamera();

        glm::mat4 getViewMatrix() override;
        glm::mat4 getPerspectiveMatrix() override;

        void setPosition(glm::vec3 position) { m_Position = position; m_Changed = true; }

        void setYaw(float yaw) { m_Yaw = yaw; m_Changed = true; }
        float& getYaw() { m_Changed = true; return m_Yaw; }

        void setPitch(float pitch) { m_Pitch = pitch; m_Changed = true; }
        float& getPitch() { m_Changed = true; return m_Pitch; }

        void setVFOV(float vFOV) { m_VFOV = vFOV; m_Changed = true; }
        float& getVFOV() { m_Changed = true; return m_VFOV; }

        glm::vec3 getUpVector() { return m_Up; }
        glm::vec3 getRightVector() { return m_Right; }
        glm::vec3 getFrontVector() { return m_Front; }
    private:
        float m_Yaw = 0.0f;
        float m_Pitch = 0.0f;
        float m_VFOV = 45.0f;

        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_Front;

        bool m_Changed = false;
    private:
        void updateCameraVectors();
    };
}
