#include "aupch.h"
#include "PerspectiveCamera.h"

namespace Aurora {
	PerspectiveCamera::PerspectiveCamera() {
		SetLens(0.25f * MathHelper::Pi, 1.0f, 1.0f, 1000.0f);
	}

	DirectX::XMVECTOR PerspectiveCamera::GetPosition() const {
		return XMLoadFloat3(&m_Position);
	}

	DirectX::XMFLOAT3 PerspectiveCamera::GetPosition3f() const {
		return m_Position;
	}

	void PerspectiveCamera::SetPosition(float x, float y, float z) {
		m_Position = DirectX::XMFLOAT3(x, y, z);
		m_ViewDirty = true;
	}

	void PerspectiveCamera::SetPosition(const DirectX::XMFLOAT3& v) {
		m_Position = v;
		m_ViewDirty = true;
	}

	DirectX::XMVECTOR PerspectiveCamera::GetRight() const {
		return XMLoadFloat3(&m_Right);
	}

	DirectX::XMFLOAT3 PerspectiveCamera::GetRight3f() const {
		return m_Right;
	}

	DirectX::XMVECTOR PerspectiveCamera::GetUp() const {
		return XMLoadFloat3(&m_Up);
	}

	DirectX::XMFLOAT3 PerspectiveCamera::GetUp3f() const {
		return m_Up;
	}

	DirectX::XMVECTOR PerspectiveCamera::GetLook() const {
		return XMLoadFloat3(&m_Look);
	}

	DirectX::XMFLOAT3 PerspectiveCamera::GetLook3f() const {
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
		// cache properties
		m_FovY = fovY;
		m_Aspect = aspect;
		m_NearZ = zn;
		m_FarZ = zf;

		m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
		m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

		DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
		XMStoreFloat4x4(&m_Proj, P);
	}

	void PerspectiveCamera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp) {
		DirectX::XMVECTOR L = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target, pos));
		DirectX::XMVECTOR R = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, L));
		DirectX::XMVECTOR U = DirectX::XMVector3Cross(L, R);

		XMStoreFloat3(&m_Position, pos);
		XMStoreFloat3(&m_Look, L);
		XMStoreFloat3(&m_Right, R);
		XMStoreFloat3(&m_Up, U);

		m_ViewDirty = true;
	}

	void PerspectiveCamera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up) {
		DirectX::XMVECTOR P = XMLoadFloat3(&pos);
		DirectX::XMVECTOR T = XMLoadFloat3(&target);
		DirectX::XMVECTOR U = XMLoadFloat3(&up);

		LookAt(P, T, U);

		m_ViewDirty = true;
	}

	DirectX::XMMATRIX PerspectiveCamera::GetView() const {
		assert(!m_ViewDirty);
		return XMLoadFloat4x4(&m_View);
	}

	DirectX::XMMATRIX PerspectiveCamera::GetProj() const {
		return XMLoadFloat4x4(&m_Proj);
	}

	DirectX::XMFLOAT4X4 PerspectiveCamera::GetView4x4f() const {
		assert(!m_ViewDirty);
		return m_View;
	}

	DirectX::XMFLOAT4X4 PerspectiveCamera::GetProj4x4f() const {
		return m_Proj;
	}

	void PerspectiveCamera::Strafe(float d) {
		// m_Position += d*m_Right
		DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
		DirectX::XMVECTOR r = XMLoadFloat3(&m_Right);
		DirectX::XMVECTOR p = XMLoadFloat3(&m_Position);
		XMStoreFloat3(&m_Position, DirectX::XMVectorMultiplyAdd(s, r, p));

		m_ViewDirty = true;
	}

	void PerspectiveCamera::Walk(float d) {
		// m_Position += d*m_Look
		DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
		DirectX::XMVECTOR l = XMLoadFloat3(&m_Look);
		DirectX::XMVECTOR p = XMLoadFloat3(&m_Position);
		XMStoreFloat3(&m_Position, DirectX::XMVectorMultiplyAdd(s, l, p));

		m_ViewDirty = true;
	}

	void PerspectiveCamera::Rise(float d) {
		// Move along the up vector
		//DirectX::XMVECTOR up = XMLoadFloat3(&m_Up);
		DirectX::XMVECTOR up = XMLoadFloat3(&m_WorldUp);
		DirectX::XMVECTOR pos = XMLoadFloat3(&m_Position);
		pos = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorReplicate(d), up, pos);
		XMStoreFloat3(&m_Position, pos);

		m_ViewDirty = true;
	}

	void PerspectiveCamera::Pitch(float angle) {
		// Rotate up and look vector about the right vector.

		DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

		XMStoreFloat3(&m_Up, DirectX::XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, DirectX::XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));

		m_ViewDirty = true;
	}

	void PerspectiveCamera::Yaw(float angle) {
		// Rotate the basis vectors about the world y-axis.

		DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

		XMStoreFloat3(&m_Right, DirectX::XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
		XMStoreFloat3(&m_Up, DirectX::XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, DirectX::XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));

		m_ViewDirty = true;
	}

	void PerspectiveCamera::SetRotation(float pitch, float yaw) {
		// for roll, we need a 3rd param

		float x = cosf(pitch) * cosf(yaw);
		float y = sinf(pitch);
		float z = cosf(pitch) * sinf(yaw);

		DirectX::XMVECTOR L = DirectX::XMVectorSet(x, y, z, 0.0f);
		L = DirectX::XMVector3Normalize(L);

		DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		DirectX::XMVECTOR R = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, L));
		DirectX::XMVECTOR U = DirectX::XMVector3Cross(L, R);

		// if roll needed
		//if (roll != 0.0f) {
		//	DirectX::XMMATRIX rollMat = DirectX::XMMatrixRotationAxis(L, roll);
		//	R = DirectX::XMVector3TransformNormal(R, rollMat);
		//	U = DirectX::XMVector3TransformNormal(U, rollMat);
		//}

		XMStoreFloat3(&m_Look, L);
		XMStoreFloat3(&m_Right, R);
		XMStoreFloat3(&m_Up, U);

		m_ViewDirty = true;
	}

	void PerspectiveCamera::UpdateViewMatrix() {
		if (m_ViewDirty) {
			DirectX::XMVECTOR R = XMLoadFloat3(&m_Right);
			DirectX::XMVECTOR U = XMLoadFloat3(&m_Up);
			DirectX::XMVECTOR L = XMLoadFloat3(&m_Look);
			DirectX::XMVECTOR P = XMLoadFloat3(&m_Position);

			// Keep camera's axes orthogonal to each other and of unit length.
			L = DirectX::XMVector3Normalize(L);
			U = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(L, R));

			// U, L already ortho-normal, so no need to normalize cross product.
			R = DirectX::XMVector3Cross(U, L);

			// Fill in the view matrix entries.
			float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, R));
			float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, U));
			float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, L));

			XMStoreFloat3(&m_Right, R);
			XMStoreFloat3(&m_Up, U);
			XMStoreFloat3(&m_Look, L);

			m_View(0, 0) = m_Right.x;
			m_View(1, 0) = m_Right.y;
			m_View(2, 0) = m_Right.z;
			m_View(3, 0) = x;

			m_View(0, 1) = m_Up.x;
			m_View(1, 1) = m_Up.y;
			m_View(2, 1) = m_Up.z;
			m_View(3, 1) = y;

			m_View(0, 2) = m_Look.x;
			m_View(1, 2) = m_Look.y;
			m_View(2, 2) = m_Look.z;
			m_View(3, 2) = z;

			m_View(0, 3) = 0.0f;
			m_View(1, 3) = 0.0f;
			m_View(2, 3) = 0.0f;
			m_View(3, 3) = 1.0f;

			m_ViewDirty = false;
		}
	}
}
