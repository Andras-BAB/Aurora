#pragma once

#include <DirectXMath.h>

namespace math {
	namespace dx_bridge {
		inline DirectX::XMMATRIX Load(const Mat4& mat) {
			return DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&mat));
		}

		inline void Store(Mat4& out, DirectX::XMMATRIX mat) {
			DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&out), mat);
		}
	}

	inline Mat4 Mat4::Identity() {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixIdentity());
		return res;
	}

	inline Mat4 Mat4::Translation(const Vec3& t) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixTranslation(t.x, t.y, t.z));
		return res;
	}

	inline Mat4 Mat4::Scale(const Vec3& s) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixScaling(s.x, s.y, s.z));
		return res;
	}

	inline Mat4 Mat4::RotationX(float angle) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixRotationX(angle));
		return res;
	}

	inline Mat4 Mat4::RotationY(float angle) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixRotationY(angle));
		return res;
	}

	inline Mat4 Mat4::RotationZ(float angle) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixRotationZ(angle));
		return res;
	}

	inline Mat4 Mat4::RotateRollPitchYaw(const Vec3& rotation) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
		return res;
	}

	inline Mat4 Mat4::LookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixLookAtLH(dx_bridge::Load(eye), dx_bridge::Load(target), dx_bridge::Load(up)));
		return res;
	}

	inline Mat4 Mat4::PerspectiveFov(float fovY, float aspect, float zNear, float zFar) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, zNear, zFar));
		return res;
	}

	inline Mat4 Mat4::Ortho(float width, float height, float zNear, float zFar) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixOrthographicLH(width, height, zNear, zFar));
		return res;
	}

	inline Mat4 Mat4::Rotation(const Quat& q) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixRotationQuaternion(dx_bridge::Load(q)));
		return res;
	}

	inline Mat4 Mat4::Inverse(const Mat4& m) {
		Mat4 res;
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(dx_bridge::Load(m));
		dx_bridge::Store(res, DirectX::XMMatrixInverse(&det, dx_bridge::Load(m)));
		return res;
	}

	inline bool Mat4::Decompose(Vec3& outTranslation, Quat& outRotation, Vec3& outScale) const {
		DirectX::XMVECTOR s, r, t;
		bool success = DirectX::XMMatrixDecompose(&s, &r, &t, dx_bridge::Load(*this));

		if (success) {
			dx_bridge::Store(outScale, s);
			dx_bridge::Store(outRotation, r);
			dx_bridge::Store(outTranslation, t);
		}
		return success;
	}

	inline Mat4 Mat4::Transpose(const Mat4& m) {
		Mat4 res;
		dx_bridge::Store(res, DirectX::XMMatrixTranspose(dx_bridge::Load(m)));
		return res;
	}

	inline bool Mat4::IsEqual(const Mat4& a, const Mat4& b) {
		DirectX::XMMATRIX matA = dx_bridge::Load(a);
		DirectX::XMMATRIX matB = dx_bridge::Load(b);

		return DirectX::XMVector4Equal(matA.r[0], matB.r[0]) &&
			DirectX::XMVector4Equal(matA.r[1], matB.r[1]) &&
			DirectX::XMVector4Equal(matA.r[2], matB.r[2]) &&
			DirectX::XMVector4Equal(matA.r[3], matB.r[3]);
	}

	inline Mat4& Mat4::operator*=(const Mat4& rhs) {
		dx_bridge::Store(*this, DirectX::XMMatrixMultiply(dx_bridge::Load(*this), dx_bridge::Load(rhs)));
		return *this;
	}

	inline bool Mat4::operator==(const Mat4& rhs) const {
		return IsEqual(*this, rhs);
	}
}
