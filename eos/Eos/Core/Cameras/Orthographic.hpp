#pragma once

#include "Eos/EosPCH.hpp"
#include "Eos/Core/Cameras/Base.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace Eos
{
    class EOS_API OrthographicCamera : public BaseCamera
    {
    public:
        OrthographicCamera();
        OrthographicCamera(glm::vec2 windowSize);
        OrthographicCamera(OrthographicCamera& camera) = default;
        /* OrthographicCamera(OrthographicCamera&& camera) = default; */

        ~OrthographicCamera();

        glm::mat4 getViewMatrix() override;
        glm::mat4 getPerspectiveMatrix() override;
    };
}
