#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace math {
	namespace glm_bridge {
		inline const glm::vec3& to_glm(const Vec3& v) {
			return *reinterpret_cast<const glm::vec3*>(&v);
		}
	}

	inline Vec3& Vec3::operator+=(const Vec3& rhs) {
		x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}

	inline Vec3& Vec3::operator-=(const Vec3& rhs) {
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
		return *this;
	}

	inline Vec3& Vec3::operator*=(float s) {
		x *= s; y *= s; z *= s;
		return *this;
	}

	inline float Vec3::dot(const Vec3& a, const Vec3& b) {
		return glm::dot(glm_bridge::to_glm(a), glm_bridge::to_glm(b));
	}

	inline Vec3 Vec3::cross(const Vec3& a, const Vec3& b) {
		glm::vec3 cross_res = glm::cross(glm_bridge::to_glm(a), glm_bridge::to_glm(b));
		return Vec3(cross_res.x, cross_res.y, cross_res.z);
	}

	inline Vec3 Vec3::normalize(const Vec3& a) {
		glm::vec3 norm_res = glm::normalize(glm_bridge::to_glm(a));
		return Vec3(norm_res.x, norm_res.y, norm_res.z);
	}
}
