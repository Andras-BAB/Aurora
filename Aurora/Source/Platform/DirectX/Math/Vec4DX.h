#pragma once

#include "Aurora/Math/Config.h"

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH

#include <DirectXMath.h>

namespace math {
	using namespace DirectX;

	// Use with alignas(16)
	struct alignas(16) Vec4 {
		XMVECTOR v;

		Vec4() : v(XMVectorZero()) {}
		Vec4(float x, float y, float z, float w)
			: v(XMVectorSet(x, y, z, w)) {
		}

		explicit Vec4(XMVECTOR vec) : v(vec) {}

		operator XMVECTOR() const { return v; }

		float x() const { return XMVectorGetX(v); }
		float y() const { return XMVectorGetY(v); }
		float z() const { return XMVectorGetZ(v); }
		float w() const { return XMVectorGetW(v); }

		Vec4& operator+=(const Vec4& rhs) {
			v = XMVectorAdd(v, rhs.v);
			return *this;
		}

		Vec4& operator-=(const Vec4& rhs) {
			v = XMVectorSubtract(v, rhs.v);
			return *this;
		}

		Vec4& operator*=(float s) {
			v = XMVectorScale(v, s);
			return *this;
		}

		friend Vec4 operator+(Vec4 a, const Vec4& b) { a += b; return a; }
		friend Vec4 operator-(Vec4 a, const Vec4& b) { a -= b; return a; }
		friend Vec4 operator*(Vec4 a, float s) { a *= s; return a; }
		friend Vec4 operator*(float s, Vec4 a) { a *= s; return a; }

		static float dot(const Vec4& a, const Vec4& b) {
			return XMVectorGetX(XMVector4Dot(a.v, b.v));
		}

		static Vec4 normalize(const Vec4& a) {
			return Vec4(XMVector4Normalize(a.v));
		}
	};
}

#endif#pragma once
