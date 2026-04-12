#pragma once

#include "Aurora/Math/Config.h"

namespace math {
	struct Vec3 {
		float x, y, z;

		Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
		Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

		static inline Vec3 Zero() { return Vec3(0.0f, 0.0f, 0.0f); }
		static inline Vec3 One() { return Vec3(1.0f, 1.0f, 1.0f); }
		static inline Vec3 Up() { return Vec3(0.0f, 1.0f, 0.0f); }
		static inline Vec3 Right() { return Vec3(1.0f, 0.0f, 0.0f); }
		static inline Vec3 Forward() { return Vec3(0.0f, 0.0f, 1.0f); }

		inline Vec3& operator+=(const Vec3& rhs);
		inline Vec3& operator-=(const Vec3& rhs);
		inline Vec3& operator*=(float s);
		inline Vec3& operator/=(float s);

		static inline float Dot(const Vec3& a, const Vec3& b);
		static inline Vec3 Cross(const Vec3& a, const Vec3& b);
		static inline Vec3 Normalize(const Vec3& a);

		inline Vec3 operator-() const {
			return Vec3(-x, -y, -z);
		}
	};

	inline Vec3 operator+(Vec3 a, const Vec3& b) { a += b; return a; }
	inline Vec3 operator-(Vec3 a, const Vec3& b) { a -= b; return a; }
	inline Vec3 operator*(Vec3 a, float s) { a *= s; return a; }
	inline Vec3 operator*(float s, Vec3 a) { a *= s; return a; }
	inline Vec3 operator/(Vec3 a, float s) { a /= s; return a; }
}

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH
	#include "Platform/DirectX/Math/Vec3DX.inl"
#elif MATH_BACKEND == MATH_BACKEND_GLM
	#include "Platform/glm/Vec3Glm.h"
#endif
