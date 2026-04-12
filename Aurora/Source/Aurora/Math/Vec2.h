#pragma once

#include "Aurora/Math/Config.h"

namespace math {
	struct Vec2 {
		float x, y;

		Vec2() : x(0.0f), y(0.0f) {}
		Vec2(float _x, float _y) : x(_x), y(_y) {}

		static inline Vec2 Zero() { return Vec2(0.0f, 0.0f); }
		static inline Vec2 One() { return Vec2(1.0f, 1.0f); }

		inline Vec2& operator+=(const Vec2& rhs);
		inline Vec2& operator-=(const Vec2& rhs);
		inline Vec2& operator*=(float s);
		inline Vec2& operator/=(float s);

		static inline float Dot(const Vec2& a, const Vec2& b);
		//static inline float Cross(const Vec2& a, const Vec2& b);
		static inline Vec2 Normalize(const Vec2& a);

		inline Vec2 operator-() const {
			return Vec2(-x, -y);
		}
	};

	inline Vec2 operator+(Vec2 a, const Vec2& b) { a += b; return a; }
	inline Vec2 operator-(Vec2 a, const Vec2& b) { a -= b; return a; }
	inline Vec2 operator*(Vec2 a, float s) { a *= s; return a; }
	inline Vec2 operator*(float s, Vec2 a) { a *= s; return a; }
	inline Vec2 operator/(Vec2 a, float s) { a /= s; return a; }
}

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH
	#include "Platform/DirectX/Math/Vec2DX.inl"
#elif MATH_BACKEND == MATH_BACKEND_GLM
	#include "Platform/glm/Vec3Glm.h"
#endif
