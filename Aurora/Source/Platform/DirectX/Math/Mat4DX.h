#pragma once
#include "Aurora/Math/Config.h"

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH

#include <DirectXMath.h>

#include "Vec3DX.h"

namespace math {
	using namespace DirectX;

	struct alignas(16) Mat4 {
		XMMATRIX m;

		Mat4() : m(XMMatrixIdentity()) {}
		explicit Mat4(const XMMATRIX& mat) : m(mat) {}
		explicit Mat4(const XMFLOAT4X4& mat) {
			m = XMLoadFloat4x4(&mat);
		}

		operator XMMATRIX() const { return m; }
		operator XMFLOAT4X4() const {
			XMFLOAT4X4 dest;
			XMStoreFloat4x4(&dest, m);
			return dest;
		}

		static Mat4 Identity() {
			return Mat4(XMMatrixIdentity());
		}

		static Mat4 Translation(const Vec3& t) {
			return Mat4(XMMatrixTranslation(t.x(), t.y(), t.z()));
		}

		static Mat4 Scale(const Vec3& s) {
			return Mat4(XMMatrixScaling(s.x(), s.y(), s.z()));
		}

		static Mat4 RotationX(float angle) {
			return Mat4(XMMatrixRotationX(angle));
		}

		static Mat4 RotationY(float angle) {
			return Mat4(XMMatrixRotationY(angle));
		}

		static Mat4 RotationZ(float angle) {
			return Mat4(XMMatrixRotationZ(angle));
		}

		static Mat4 RotateRollPitchYaw(const Vec3& rotation) {
			// rotation.x = pitch, rotation.y = yaw, rotation.z = roll
			return Mat4(XMMatrixRotationRollPitchYaw(rotation.x(), rotation.y(), rotation.z()));
		}

		// LookAt (left-handed, like D3D)
		static Mat4 look_at_lh(const Vec3& eye, const Vec3& target, const Vec3& up) {
			return Mat4(XMMatrixLookAtLH(eye.v, target.v, up.v));
		}

		// Perspective (left-handed, D3D style)
		static Mat4 perspective_fov_lh(float fovY, float aspect, float zNear, float zFar) {
			return Mat4(XMMatrixPerspectiveFovLH(fovY, aspect, zNear, zFar));
		}

		// Ortho
		static Mat4 ortho_lh(float width, float height, float zNear, float zFar) {
			return Mat4(XMMatrixOrthographicLH(width, height, zNear, zFar));
		}

		Mat4& operator*=(const Mat4& rhs) {
			m = XMMatrixMultiply(m, rhs.m);
			return *this;
		}

		friend Mat4 operator*(Mat4 a, const Mat4& b) {
			a *= b;
			return a;
		}
	};
}

#endif

