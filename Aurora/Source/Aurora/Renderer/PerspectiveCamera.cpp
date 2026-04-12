#include "aupch.h"

#include "PerspectiveCamera.h"

namespace Aurora {
	PerspectiveCamera::PerspectiveCamera() {
		SetLens(0.25f * math::PI, 1.0f, 1.0f, 1000.0f);
	}

	math::Vec3 PerspectiveCamera::GetPosition() const {
		return m_Position;
	}

	void PerspectiveCamera::SetPosition(float x, float y, float z) {
		m_Position = math::Vec3(x, y, z);
		m_ViewDirty = true;
	}

	void PerspectiveCamera::SetPosition(const math::Vec3& v) {
		m_Position = v;
		m_ViewDirty = true;
	}

	math::Vec3 PerspectiveCamera::GetRight() const {
		return m_Right;
	}

	math::Vec3 PerspectiveCamera::GetUp() const {
		return m_Up;
	}

	math::Vec3 PerspectiveCamera::GetLook() const {
		return m_Look;
	}

	float PerspectiveCamera::GetNearZ() const {
		return m_NearZ;
	}

	float PerspectiveCamera::GetFarZ() const {
		return m_FarZ;
	}

	float PerspectiveCamera::GetAspect() const {
		return m_Aspect;
	}

	float PerspectiveCamera::GetFovY() const {
		return m_FovY;
	}

	float PerspectiveCamera::GetFovX() const {
		float halfWidth = 0.5f * GetNearWindowWidth();
		return 2.0f * atan(halfWidth / m_NearZ);
	}

	float PerspectiveCamera::GetNearWindowWidth() const {
		return m_Aspect * m_NearWindowHeight;
	}

	float PerspectiveCamera::GetNearWindowHeight() const {
		return m_NearWindowHeight;
	}

	float PerspectiveCamera::GetFarWindowWidth() const {
		return m_Aspect * m_FarWindowHeight;
	}

	float PerspectiveCamera::GetFarWindowHeight() const {
		return m_FarWindowHeight;
	}

	void PerspectiveCamera::SetLens(float fovY, float aspect, float zn, float zf) {
		m_FovY = fovY;
		m_Aspect = aspect;
		m_NearZ = zn;
		m_FarZ = zf;

		m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
		m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

		float halfFovTan = tanf(0.5f * m_FovY);
		float yScale = 1.0f / halfFovTan;
		float xScale = yScale / m_Aspect;

		// reversed-Z for better precision (and infinite view distance)
		math::Mat4 P(
			xScale, 0.0f, 0.0f, 0.0f,
			0.0f, yScale, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, zn, 0.0f
		);

		m_Proj = P;
	}

	void PerspectiveCamera::LookAt(const math::Vec3& pos, const math::Vec3& target, const math::Vec3& up) {
		math::Vec3 L = math::Vec3::Normalize(target - pos);
		math::Vec3 R = math::Vec3::Normalize(math::Vec3::Cross(up, L));
		math::Vec3 U = math::Vec3::Cross(L, R);

		m_Position = pos;
		m_Look = L;
		m_Right = R;
		m_Up = U;

		m_ViewDirty = true;
	}

	math::Mat4 PerspectiveCamera::GetView() const {
		return m_View;
	}

	math::Mat4 PerspectiveCamera::GetProj() const {
		return m_Proj;
	}

	void PerspectiveCamera::Strafe(float d) {
		m_Position += m_Right * d;
		m_ViewDirty = true;
	}

	void PerspectiveCamera::Walk(float d) {
		m_Position += m_Look * d;
		m_ViewDirty = true;
	}

	void PerspectiveCamera::Rise(float d) {
		m_Position += m_WorldUp * d;
		m_ViewDirty = true;
	}

	void PerspectiveCamera::Pitch(float angle) {
		math::Quat q = math::Quat::FromAxisAngle(m_Right, angle);

		m_Up = q * m_Up;
		m_Look = q * m_Look;

		m_ViewDirty = true;
	}

	void PerspectiveCamera::Yaw(float angle) {
		math::Quat q = math::Quat::FromAxisAngle(math::Vec3::Up(), angle);

		m_Right = q * m_Right;
		m_Up = q * m_Up;
		m_Look = q * m_Look;

		m_ViewDirty = true;
	}

	void PerspectiveCamera::Roll(float angle) {
		math::Quat q = math::Quat::FromAxisAngle(m_Look, angle);

		m_Right = q * m_Right;
		m_Up = q * m_Up;

		m_ViewDirty = true;
	}

	void PerspectiveCamera::SetRotation(float roll, float pitch, float yaw) {
		math::Quat q = math::Quat::FromEuler(math::Vec3(pitch, yaw, roll));
		SetRotation(q);
	}

	void PerspectiveCamera::SetRotation(const math::Quat& rotation) {
		m_Right = rotation * math::Vec3::Right();
		m_Up = rotation * math::Vec3::Up();
		m_Look = rotation * math::Vec3::Forward();

		m_ViewDirty = true;
	}

	void PerspectiveCamera::UpdateViewMatrix() {
		if (m_ViewDirty) {
			m_Look = math::Vec3::Normalize(m_Look);
			m_Up = math::Vec3::Normalize(math::Vec3::Cross(m_Look, m_Right));
			
			m_Right = math::Vec3::Cross(m_Up, m_Look);

			float x = -math::Vec3::Dot(m_Position, m_Right);
			float y = -math::Vec3::Dot(m_Position, m_Up);
			float z = -math::Vec3::Dot(m_Position, m_Look);

			m_View.m[0][0] = m_Right.x;
			m_View.m[1][0] = m_Right.y;
			m_View.m[2][0] = m_Right.z;
			m_View.m[3][0] = x;

			m_View.m[0][1] = m_Up.x;
			m_View.m[1][1] = m_Up.y;
			m_View.m[2][1] = m_Up.z;
			m_View.m[3][1] = y;

			m_View.m[0][2] = m_Look.x;
			m_View.m[1][2] = m_Look.y;
			m_View.m[2][2] = m_Look.z;
			m_View.m[3][2] = z;

			m_View.m[0][3] = 0.0f;
			m_View.m[1][3] = 0.0f;
			m_View.m[2][3] = 0.0f;
			m_View.m[3][3] = 1.0f;

			m_ViewDirty = false;
		}
	}
}
