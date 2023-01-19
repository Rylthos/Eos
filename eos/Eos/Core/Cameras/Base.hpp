#pragma once

#include "Eos/EosPCH.hpp"

namespace Eos
{
    class EOS_API BaseCamera
    {
    public:
        BaseCamera();
        BaseCamera(glm::vec2 windowSize);

        virtual ~BaseCamera() {}

        virtual glm::mat4 getViewMatrix() { return glm::mat4(1.0f); }
        virtual glm::mat4 getPerspectiveMatrix() { return glm::mat4(1.0f); }

        void setPosition(glm::vec3 position) { m_Position = position; }
        glm::vec3& getPosition() { return m_Position; }

        void setWindowSize(glm::vec2 windowSize) { m_WindowSize = windowSize; }

        void setNearClippingPlane(float value) { m_NearClipping = value; }
        void setFarClippingPlane(float value) { m_FarClipping = value; }
    protected:
        glm::vec3 m_WorldUp { 0.0f, 1.0f, 0.0f }; // Perspective matrix flips y
        glm::vec3 m_WorldRight { 1.0f, 0.0f, 0.0f };
        glm::vec3 m_WorldFront { 0.0f, 0.0f, 1.0f };

        glm::vec3 m_Position;
        glm::vec2 m_WindowSize;

        float m_NearClipping = 0.0f;
        float m_FarClipping = 1.0f;
    };
}
