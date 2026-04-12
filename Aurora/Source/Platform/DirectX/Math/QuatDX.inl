#pragma once

#include <DirectXMath.h>

namespace math {
	namespace dx_bridge {
		inline DirectX::XMVECTOR Load(const Quat& q) {
			return DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&q));
		}

		inline void Store(Quat& out, DirectX::XMVECTOR v) {
			DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&out), v);
		}
	}

	inline Quat Quat::FromEuler(const Vec3& eulerAngles) {
		Quat res;
		dx_bridge::Store(res, DirectX::XMQuaternionRotationRollPitchYaw(eulerAngles.x, eulerAngles.y, eulerAngles.z));
		return res;
	}

	inline Quat Quat::FromAxisAngle(const Vec3& axis, float angle) {
		Quat res;
		dx_bridge::Store(res, DirectX::XMQuaternionRotationAxis(dx_bridge::Load(axis), angle));
		return res;
	}

	inline Quat& Quat::operator*=(const Quat& rhs) {
		dx_bridge::Store(*this, DirectX::XMQuaternionMultiply(dx_bridge::Load(*this), dx_bridge::Load(rhs)));
		return *this;
	}

	inline float Quat::Dot(const Quat& a, const Quat& b) {
		return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(dx_bridge::Load(a), dx_bridge::Load(b)));
	}

	inline Quat Quat::Normalize(const Quat& q) {
		Quat res;
		dx_bridge::Store(res, DirectX::XMQuaternionNormalize(dx_bridge::Load(q)));
		return res;
	}

	inline Quat Quat::Inverse(const Quat& q) {
		Quat res;
		dx_bridge::Store(res, DirectX::XMQuaternionInverse(dx_bridge::Load(q)));
		return res;
	}

	inline Quat Quat::Conjugate(const Quat& q) {
		Quat res;
		dx_bridge::Store(res, DirectX::XMQuaternionConjugate(dx_bridge::Load(q)));
		return res;
	}

	inline Quat Quat::Slerp(const Quat& a, const Quat& b, float t) {
		Quat res;
		dx_bridge::Store(res, DirectX::XMQuaternionSlerp(dx_bridge::Load(a), dx_bridge::Load(b), t));
		return res;
	}

	inline Vec3 operator*(const Quat& q, const Vec3& v) {
		Vec3 res;
		dx_bridge::Store(res, DirectX::XMVector3Rotate(dx_bridge::Load(v), dx_bridge::Load(q)));
		return res;
	}
}