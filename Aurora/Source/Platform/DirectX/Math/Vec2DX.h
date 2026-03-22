#pragma once

#include "Aurora/Math/Config.h"

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH

#include <DirectXMath.h>

namespace math {
	using namespace DirectX;

	// Use with alignas(16)
	struct alignas(16) Vec2 {
		XMVECTOR v;

		Vec2() : v(XMVectorZero()) {}
		Vec2(float x, float y)
			: v(XMVectorSet(x, y, 0.0f, 0.0f)) {
		}

		explicit Vec2(XMVECTOR vec) : v(vec) {}

		operator XMVECTOR() const { return v; }

		float x() const { return XMVectorGetX(v); }
		float y() const { return XMVectorGetY(v); }

		Vec2& operator+=(const Vec2& rhs) {
			v = XMVectorAdd(v, rhs.v);
			return *this;
		}

		Vec2& operator-=(const Vec2& rhs) {
			v = XMVectorSubtract(v, rhs.v);
			return *this;
		}

		Vec2& operator*=(float s) {
			v = XMVectorScale(v, s);
			return *this;
		}

		friend Vec2 operator+(Vec2 a, const Vec2& b) { a += b; return a; }
		friend Vec2 operator-(Vec2 a, const Vec2& b) { a -= b; return a; }
		friend Vec2 operator*(Vec2 a, float s) { a *= s; return a; }
		friend Vec2 operator*(float s, Vec2 a) { a *= s; return a; }

		static float dot(const Vec2& a, const Vec2& b) {
			return XMVectorGetX(XMVector2Dot(a.v, b.v));
		}

		static Vec2 normalize(const Vec2& a) {
			return Vec2(XMVector2Normalize(a.v));
		}

		static float length(const Vec2& a) {
			return XMVectorGetX(XMVector2Length(a.v));
		}
	};
}

#endif