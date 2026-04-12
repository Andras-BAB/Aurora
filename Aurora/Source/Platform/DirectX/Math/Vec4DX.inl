#pragma once

#include <DirectXMath.h>

namespace math {
	namespace dx_bridge {
		inline DirectX::XMVECTOR Load(const Vec4& v) {
			return DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&v));
		}

		inline void Store(Vec4& out, DirectX::XMVECTOR v) {
			DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&out), v);
		}
	}

	inline Vec4& Vec4::operator+=(const Vec4& rhs) {
		dx_bridge::Store(*this, DirectX::XMVectorAdd(dx_bridge::Load(*this), dx_bridge::Load(rhs)));
		return *this;
	}

	inline Vec4& Vec4::operator-=(const Vec4& rhs) {
		dx_bridge::Store(*this, DirectX::XMVectorSubtract(dx_bridge::Load(*this), dx_bridge::Load(rhs)));
		return *this;
	}

	inline Vec4& Vec4::operator*=(float s) {
		dx_bridge::Store(*this, DirectX::XMVectorScale(dx_bridge::Load(*this), s));
		return *this;
	}

	inline Vec4& Vec4::operator/=(float s) {
		float inv = 1.0f / s;
		dx_bridge::Store(*this, DirectX::XMVectorScale(dx_bridge::Load(*this), inv));
		return *this;
	}

	inline float Vec4::Dot(const Vec4& a, const Vec4& b) {
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(dx_bridge::Load(a), dx_bridge::Load(b)));
	}

	inline Vec4 Vec4::Normalize(const Vec4& a) {
		Vec4 res;
		dx_bridge::Store(res, DirectX::XMVector4Normalize(dx_bridge::Load(a)));
		return res;
	}
}
