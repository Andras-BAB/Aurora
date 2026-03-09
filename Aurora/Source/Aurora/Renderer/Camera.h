#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Aurora {
    
    class Camera {
    public:
        Camera() = default;
        Camera(const glm::mat4& projection) : m_ProjectionMatrix(projection) {}
        virtual ~Camera() = default;

        const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
    
    protected:
        glm::mat4 m_ProjectionMatrix{};
    };
    
}
