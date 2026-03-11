#pragma once

#include "Math/Config.h"

#if MATH_BACKEND == MATH_BACKEND_GLM

#include <glm/glm.hpp>

namespace math {
	struct Vec4 {
		glm::vec4 v;

		Vec4() : v(0.0f) {}
		Vec4(float x, float y, float z, float w) : v(x, y, z, w) {}
		explicit Vec4(const glm::vec4& other) : v(other) {}

		operator glm::vec4() const { return v; }

		float x() const { return v.x; }
		float y() const { return v.y; }
		float z() const { return v.z; }
		float w() const { return v.w; }

		Vec4& operator+=(const Vec4& rhs) { v += rhs.v; return *this; }
		Vec4& operator-=(const Vec4& rhs) { v -= rhs.v; return *this; }
		Vec4& operator*=(float s) { v *= s; return *this; }

		friend Vec4 operator+(Vec4 a, const Vec4& b) { a += b; return a; }
		friend Vec4 operator-(Vec4 a, const Vec4& b) { a -= b; return a; }
		friend Vec4 operator*(Vec4 a, float s) { a *= s; return a; }
		friend Vec4 operator*(float s, Vec4 a) { a *= s; return a; }

		static float dot(const Vec4& a, const Vec4& b) {
			return glm::dot(a.v, b.v);
		}

		static Vec4 normalize(const Vec4& a) {
			return Vec4(glm::normalize(a.v));
		}
	};
}

#endif
