#pragma once
#include "Math/Config.h"

#if MATH_BACKEND == MATH_BACKEND_GLM

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace math {
	struct Mat4 {
		glm::mat4 m;

		Mat4() : m(1.0f) {}
		explicit Mat4(const glm::mat4& mat) : m(mat) {}

		operator glm::mat4() const { return m; }

		static Mat4 identity() {
			return Mat4(glm::mat4(1.0f));
		}

		static Mat4 translation(const Vec3& t) {
			return Mat4(glm::translate(glm::mat4(1.0f), t.v));
		}

		static Mat4 scale(const Vec3& s) {
			return Mat4(glm::scale(glm::mat4(1.0f), s.v));
		}

		static Mat4 rotation_x(float angle) {
			return Mat4(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1, 0, 0)));
		}

		static Mat4 rotation_y(float angle) {
			return Mat4(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)));
		}

		static Mat4 rotation_z(float angle) {
			return Mat4(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1)));
		}

		// LookAt (GLM default: right-handed, de csinálhatsz LH verziót is)
		static Mat4 look_at(const Vec3& eye, const Vec3& target, const Vec3& up) {
			return Mat4(glm::lookAt(eye.v, target.v, up.v));
		}

		static Mat4 perspective(float fovY, float aspect, float zNear, float zFar) {
			return Mat4(glm::perspective(fovY, aspect, zNear, zFar));
		}

		Mat4& operator*=(const Mat4& rhs) {
			m = m * rhs.m;
			return *this;
		}

		friend Mat4 operator*(Mat4 a, const Mat4& b) {
			a *= b;
			return a;
		}
	};
}

#endif

