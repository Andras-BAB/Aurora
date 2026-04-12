#pragma once

#include "Camera.h"

namespace Aurora {
	
	class PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera();
		~PerspectiveCamera() override = default;

		// Get/Set world camera position.
		math::Vec3 GetPosition() const;
		void SetPosition(float x, float y, float z);
		void SetPosition(const math::Vec3& v);

		// Get camera basis vectors.
		math::Vec3 GetRight() const;
		math::Vec3 GetUp() const;
		math::Vec3 GetLook() const;

		// Get frustum properties.
		float GetNearZ() const;
		float GetFarZ() const;
		float GetAspect() const;
		float GetFovY() const;
		float GetFovX() const;

		// Get near and far plane dimensions in view space coordinates.
		float GetNearWindowWidth() const;
		float GetNearWindowHeight() const;
		float GetFarWindowWidth() const;
		float GetFarWindowHeight() const;

		// Set frustum.
		void SetLens(float fovY, float aspect, float zn, float zf);

		// Define camera space via LookAt parameters.
		void LookAt(const math::Vec3& pos, const math::Vec3& target, const math::Vec3& up);

		// Get View/Proj matrices.
		math::Mat4 GetView() const;
		math::Mat4 GetProj() const;

		// Strafe/Walk the camera a distance d.
		void Strafe(float d);
		void Walk(float d);
		void Rise(float d);

		// Rotate the camera.
		void Pitch(float angle);
		void Yaw(float angle);
		void Roll(float angle);

		void SetRotation(float roll, float pitch, float yaw);
		void SetRotation(const math::Quat& rotation);

		// After modifying camera position/orientation, call to rebuild the view matrix.
		void UpdateViewMatrix();

	private:

		// Camera coordinate system with coordinates relative to world space.
		math::Vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		math::Vec3 m_Right = { 1.0f, 0.0f, 0.0f };
		math::Vec3 m_Up = { 0.0f, 1.0f, 0.0f };
		math::Vec3 m_Look = { 0.0f, 0.0f, 1.0f };
		math::Vec3 m_WorldUp = { 0.0f, 1.0f, 0.0f };

		// Cache frustum properties.
		float m_NearZ = 0.0f;
		float m_FarZ = 0.0f;
		float m_Aspect = 0.0f;
		float m_FovY = 0.0f;
		float m_NearWindowHeight = 0.0f;
		float m_FarWindowHeight = 0.0f;

		bool m_ViewDirty = true;

		// Cache View/Proj matrices.
		math::Mat4 m_View = math::Mat4::Identity();
		math::Mat4 m_Proj = math::Mat4::Identity();
	};
	
}

