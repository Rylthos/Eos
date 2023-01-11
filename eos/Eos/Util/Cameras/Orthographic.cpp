#include "Orthographic.hpp"

namespace Eos
{
    OrthographicCamera::OrthographicCamera()
        : BaseCamera() { }

    OrthographicCamera::OrthographicCamera(glm::vec2 windowSize)
        : BaseCamera(windowSize) { }

    OrthographicCamera::~OrthographicCamera() {}

    glm::mat4 OrthographicCamera::getViewMatrix()
    {
        glm::mat4 view(1.0f);
        view = glm::translate(view, -m_Position);

        return view;
    }

    glm::mat4 OrthographicCamera::getPerspectiveMatrix()
    {
        return glm::ortho(0.0f, m_WindowSize.x, m_WindowSize.y, 0.0f);
    }
}
