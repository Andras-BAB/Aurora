#pragma once

#include <DirectXMath.h>

namespace math {
	namespace dx_bridge {
		inline DirectX::XMVECTOR Load(const Vec2& v) {
			return DirectX::XMLoadFloat2(reinterpret_cast<const DirectX::XMFLOAT2*>(&v));
		}

		inline void Store(Vec2& out, DirectX::XMVECTOR v) {
			DirectX::XMStoreFloat2(reinterpret_cast<DirectX::XMFLOAT2*>(&out), v);
		}
	}

	inline Vec2& Vec2::operator+=(const Vec2& rhs) {
		dx_bridge::Store(*this, DirectX::XMVectorAdd(dx_bridge::Load(*this), dx_bridge::Load(rhs)));
		return *this;
	}

	inline Vec2& Vec2::operator-=(const Vec2& rhs) {
		dx_bridge::Store(*this, DirectX::XMVectorSubtract(dx_bridge::Load(*this), dx_bridge::Load(rhs)));
		return *this;
	}

	inline Vec2& Vec2::operator*=(float s) {
		dx_bridge::Store(*this, DirectX::XMVectorScale(dx_bridge::Load(*this), s));
		return *this;
	}

	inline Vec2& Vec2::operator/=(float s) {
		float inv = 1.0f / s;
		dx_bridge::Store(*this, DirectX::XMVectorScale(dx_bridge::Load(*this), inv));
		return *this;
	}

	inline float Vec2::Dot(const Vec2& a, const Vec2& b) {
		return DirectX::XMVectorGetX(DirectX::XMVector2Dot(dx_bridge::Load(a), dx_bridge::Load(b)));
	}

	//inline float Vec2::Cross(const Vec2& a, const Vec2& b) {
	//	return DirectX::XMVectorGetX(DirectX::XMVector2Cross(dx_bridge::Load(a), dx_bridge::Load(b)));
	//}

	inline Vec2 Vec2::Normalize(const Vec2& a) {
		Vec2 res;
		dx_bridge::Store(res, DirectX::XMVector2Normalize(dx_bridge::Load(a)));
		return res;
	}
}
