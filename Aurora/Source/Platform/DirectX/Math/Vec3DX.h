#pragma once

#include "Aurora/Math/Config.h"

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH

#include <DirectXMath.h>

namespace math {
	using namespace DirectX;

	// Use with alignas(16)
	struct alignas(16) Vec3 {
		XMVECTOR v;

		Vec3() : v(XMVectorZero()) {}
		Vec3(float x, float y, float z)
			: v(XMVectorSet(x, y, z, 0.0f)) {
		}

		explicit Vec3(XMVECTOR vec) : v(vec) {}
		Vec3(XMFLOAT3 vec) { v = XMLoadFloat3(&vec); }

		operator XMVECTOR() const { return v; }

		float x() const { return XMVectorGetX(v); }
		float y() const { return XMVectorGetY(v); }
		float z() const { return XMVectorGetZ(v); }

		Vec3& operator+=(const Vec3& rhs) {
			v = XMVectorAdd(v, rhs.v);
			return *this;
		}

		Vec3& operator-=(const Vec3& rhs) {
			v = XMVectorSubtract(v, rhs.v);
			return *this;
		}

		Vec3& operator*=(float s) {
			v = XMVectorScale(v, s);
			return *this;
		}

		friend Vec3 operator+(Vec3 a, const Vec3& b) { a += b; return a; }
		friend Vec3 operator-(Vec3 a, const Vec3& b) { a -= b; return a; }
		friend Vec3 operator*(Vec3 a, float s) { a *= s; return a; }
		friend Vec3 operator*(float s, Vec3 a) { a *= s; return a; }

		static Vec3 cross(const Vec3& a, const Vec3& b) {
			return Vec3(XMVector3Cross(a.v, b.v));
		}

		static float dot(const Vec3& a, const Vec3& b) {
			return XMVectorGetX(XMVector3Dot(a.v, b.v));
		}

		static Vec3 normalize(const Vec3& a) {
			return Vec3(XMVector3Normalize(a.v));
		}
	};
}

#endif

