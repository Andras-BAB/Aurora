#pragma once

#include "Math/Config.h"

#if MATH_BACKEND == MATH_BACKEND_GLM

#include <glm/glm.hpp>

namespace math {
	struct Vec2 {
		glm::vec2 v;

		Vec2() : v(0.0f) {}
		Vec2(float x, float y) : v(x, y) {}
		explicit Vec2(const glm::vec2& other) : v(other) {}

		operator glm::vec2() const { return v; }

		float x() const { return v.x; }
		float y() const { return v.y; }

		Vec2& operator+=(const Vec2& rhs) { v += rhs.v; return *this; }
		Vec2& operator-=(const Vec2& rhs) { v -= rhs.v; return *this; }
		Vec2& operator*=(float s) { v *= s; return *this; }

		friend Vec2 operator+(Vec2 a, const Vec2& b) { a += b; return a; }
		friend Vec2 operator-(Vec2 a, const Vec2& b) { a -= b; return a; }
		friend Vec2 operator*(Vec2 a, float s) { a *= s; return a; }
		friend Vec2 operator*(float s, Vec2 a) { a *= s; return a; }

		static float dot(const Vec2& a, const Vec2& b) {
			return glm::dot(a.v, b.v);
		}

		static Vec2 normalize(const Vec2& a) {
			return Vec2(glm::normalize(a.v));
		}

		static float length(const Vec2& a) {
			return glm::length(a.v);
		}
	};
}

#endif
