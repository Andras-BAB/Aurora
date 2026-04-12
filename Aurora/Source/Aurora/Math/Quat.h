#pragma once

#include "Aurora/Math/Config.h"
#include "Aurora/Math/Vec3.h"

namespace math {
	struct Quat {
		float x, y, z, w;

		// identity quaternion
		Quat() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
		Quat(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

		static inline Quat Identity() { return Quat(0.0f, 0.0f, 0.0f, 1.0f); }

		// create from Euler angles (Pitch, Yaw, Roll) in radians
		static inline Quat FromEuler(const Vec3& eulerAngles);

		// create from axis-angle representation
		static inline Quat FromAxisAngle(const Vec3& axis, float angle);

		inline Quat& operator*=(const Quat& rhs);

		static inline float Dot(const Quat& a, const Quat& b);
		static inline Quat Normalize(const Quat& q);
		static inline Quat Inverse(const Quat& q);
		static inline Quat Conjugate(const Quat& q);

		static inline Quat Slerp(const Quat& a, const Quat& b, float t);

		static inline Vec3 ToEuler(const Quat& q) {
			Vec3 angles;

			// pitch
			float sinp = 2.0f * (q.w * q.x - q.y * q.z);
			if (std::abs(sinp) >= 1.0f)
				angles.x = std::copysign(3.14159265f / 2.0f, sinp);
			else
				angles.x = std::asin(sinp);

			// yaw
			float siny_cosp = 2.0f * (q.w * q.y + q.z * q.x);
			float cosy_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
			angles.y = std::atan2(siny_cosp, cosy_cosp);

			// roll
			float sinr_cosp = 2.0f * (q.w * q.z + q.x * q.y);
			float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
			angles.z = std::atan2(sinr_cosp, cosr_cosp);

			return angles;
		}
	};

	// adding rotations
	inline Quat operator*(Quat a, const Quat& b) { a *= b; return a; }

	// rotate a 3D vector with a quat
	inline Vec3 operator*(const Quat& q, const Vec3& v);
}

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH
	#include "Platform/DirectX/Math/QuatDX.inl"
#elif MATH_BACKEND == MATH_BACKEND_GLM
	#include "Platform/glm/QuatGlm.h"
#endif