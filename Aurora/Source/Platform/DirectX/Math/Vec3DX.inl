#pragma once

#include <DirectXMath.h>

#include "Aurora/Core/Assert.h"

namespace math {
	namespace dx_bridge {
		inline DirectX::XMVECTOR Load(const Vec3& v) {
			return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&v));
		}

		inline void Store(Vec3& out, DirectX::XMVECTOR v) {
			DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&out), v);
		}
	}
	
	inline Vec3& Vec3::operator+=(const Vec3& rhs) {
		dx_bridge::Store(*this, DirectX::XMVectorAdd(dx_bridge::Load(*this), dx_bridge::Load(rhs)));
		return *this;
	}

	inline Vec3& Vec3::operator-=(const Vec3& rhs) {
		dx_bridge::Store(*this, DirectX::XMVectorSubtract(dx_bridge::Load(*this), dx_bridge::Load(rhs)));
		return *this;
	}

	inline Vec3& Vec3::operator*=(float s) {
		dx_bridge::Store(*this, DirectX::XMVectorScale(dx_bridge::Load(*this), s));
		return *this;
	}

	inline Vec3& Vec3::operator/=(float s) {
		AU_CORE_ASSERT(s != 0, "Division by 0 is not permitted!");
		float inv = 1.0f / s;
		dx_bridge::Store(*this, DirectX::XMVectorScale(dx_bridge::Load(*this), inv));
		return *this;
	}

	inline float Vec3::Dot(const Vec3& a, const Vec3& b) {
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(dx_bridge::Load(a), dx_bridge::Load(b)));
	}

	inline Vec3 Vec3::Cross(const Vec3& a, const Vec3& b) {
		Vec3 res;
		dx_bridge::Store(res, DirectX::XMVector3Cross(dx_bridge::Load(a), dx_bridge::Load(b)));
		return res;
	}

	inline Vec3 Vec3::Normalize(const Vec3& a) {
		Vec3 res;
		dx_bridge::Store(res, DirectX::XMVector3Normalize(dx_bridge::Load(a)));
		return res;
	}
}
