#pragma once

#include "Math/Config.h"

#if MATH_BACKEND == MATH_BACKEND_GLM

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace math {
	struct Vec3 {
		glm::vec3 v;

		Vec3() : v(0.0f) {}
		Vec3(float x, float y, float z) : v(x, y, z) {}
		explicit Vec3(const glm::vec3& other) : v(other) {}

		operator glm::vec3() const { return v; }

		float x() const { return v.x; }
		float y() const { return v.y; }
		float z() const { return v.z; }

		Vec3& operator+=(const Vec3& rhs) { v += rhs.v; return *this; }
		Vec3& operator-=(const Vec3& rhs) { v -= rhs.v; return *this; }
		Vec3& operator*=(float s) { v *= s;     return *this; }

		friend Vec3 operator+(Vec3 a, const Vec3& b) { a += b; return a; }
		friend Vec3 operator-(Vec3 a, const Vec3& b) { a -= b; return a; }
		friend Vec3 operator*(Vec3 a, float s) { a *= s; return a; }
		friend Vec3 operator*(float s, Vec3 a) { a *= s; return a; }

		static Vec3 cross(const Vec3& a, const Vec3& b) {
			return Vec3(glm::cross(a.v, b.v));
		}

		static float dot(const Vec3& a, const Vec3& b) {
			return glm::dot(a.v, b.v);
		}

		static Vec3 normalize(const Vec3& a) {
			return Vec3(glm::normalize(a.v));
		}
	};
}

#endif

