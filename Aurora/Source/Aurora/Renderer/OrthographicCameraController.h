#pragma once

#include "Aurora/Renderer/OrthographicCamera.h"
#include "Aurora/Core/Timestep.h"
#include "Aurora/Events/ApplicationEvent.h"
#include "Aurora/Events/MouseEvent.h"

namespace Aurora {

    class OrthographicCameraController {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false);

        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);

        void OnResize(float width, float height);

        OrthographicCamera& GetCamera();
        const OrthographicCamera& GetCamera() const;

        float GetZoomLevel() const;
        void SetZoomLevel(float level);
        
    private:
        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnWindowResized(WindowResizeEvent& e);
        
    private:
        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        OrthographicCamera m_Camera;

        bool m_Rotation;

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraRotation = 0.0f;
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
    };

}
