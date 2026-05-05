#pragma once

#include "Aurora/Math/Config.h"
#include "Aurora/Math/Vec3.h"
#include "Aurora/Math/Quat.h"

namespace math {
	struct Mat4 {
		float m[4][4];

		Mat4() {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					m[i][j] = (i == j) ? 1.0f : 0.0f;
		}

		Mat4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		) {
			m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
			m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
			m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
			m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
		}

		static inline Mat4 Identity();
		static inline Mat4 Translation(const Vec3& t);
		static inline Mat4 Scale(const Vec3& s);
		static inline Mat4 RotationX(float angle);
		static inline Mat4 RotationY(float angle);
		static inline Mat4 RotationZ(float angle);

		static inline Mat4 RotateRollPitchYaw(const Vec3& rotation); // x = pitch, y = yaw, z = roll
		static inline Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
		static inline Mat4 PerspectiveFov(float fovY, float aspect, float zNear, float zFar);
		static inline Mat4 Ortho(float width, float height, float zNear, float zFar);

		static inline Mat4 Rotation(const Quat& q);
		static inline Mat4 Transpose(const Mat4& m);
		static inline Mat4 Inverse(const Mat4& m);

		bool Decompose(Vec3& outTranslation, Quat& outRotation, Vec3& outScale) const;

		static inline bool IsEqual(const Mat4& a, const Mat4& b);

		inline Mat4& operator*=(const Mat4& rhs);
		inline bool operator==(const Mat4& rhs) const;

		inline bool operator!=(const Mat4& rhs) const { return !(*this == rhs); }

		inline Vec3 GetTranslation() const {
			return Vec3(m[3][0], m[3][1], m[3][2]);
		}
	};

	inline Mat4 operator*(Mat4 a, const Mat4& b) { a *= b; return a; }
}

#if MATH_BACKEND == MATH_BACKEND_DIRECTXMATH
	#include "Platform/DirectX/Math/Mat4DX.inl"
#elif MATH_BACKEND == MATH_BACKEND_GLM
	#include "Platform/glm/Mat4Glm.h"
#endif