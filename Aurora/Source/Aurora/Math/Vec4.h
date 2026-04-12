#pragma once

#include "Aurora/Math/Config.h"

namespace math {
	struct Vec4 {
		float x, y, z, w;

		Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

		static inline Vec4 Zero() { return Vec4(0.0f, 0.0f, 0.0f, 0.0f); }
		static inline Vec4 One() { return Vec4(1.0f, 1.0f, 1.0f, 1.0f); }

		inline Vec4& operator+=(const Vec4& rhs);
		inline Vec4& operator-=(const Vec4& rhs);
		inline Vec4& operator*=(float s);
		inline Vec4& operator/=(float s);

		static inline float Dot(const Vec4& a, const Vec4& b);
		static inline Vec4 Normalize(const Vec4& a);

		inline Vec4 operator-() const {
			return Vec4(-x, -y, -z, -w);
		}
	};

	inline Vec4 operator+(Vec4 a, const Vec4& b) { a += b; return a; }
	inline Vec4 operator-(Vec4 a, const Vec4& b) { a -= b; return a; }
	inline Vec4 operator*(Vec4 a, float s) { a *= s; return a; }
	inline Vec4 operator*(float s, Vec4 a) { a *= s; return a; }
	inline Vec4 operator/(Vec4 a, float s) { a /= s; return a; }
}

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH
	#include "Platform/DirectX/Math/Vec4DX.inl"
#elif MATH_BACKEND == MATH_BACKEND_GLM
	#include "Platform/glm/Vec4Glm.h"
#endif