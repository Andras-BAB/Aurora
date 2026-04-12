//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>

#include "Aurora/Math/Math.h"

class MathHelper {
public:
	// Returns random float in [0, 1).
	static float RandF() {
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b) {
		return a + RandF() * (b - a);
	}

	static int Rand(int a, int b) {
		return a + rand() % ((b - a) + 1);
	}

	template<typename T>
	static T Min(const T& a, const T& b) {
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t) {
		return a + (b - a) * t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high) {
		return x < low ? low : (x > high ? high : x);
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	static inline math::Vec3 SphericalToCartesian(float radius, float theta, float phi) {
		return math::Vec3(radius * sinf(phi) * cosf(theta),
			radius * cosf(phi),
			radius * sinf(phi) * sinf(theta));
	}

	static DirectX::XMVECTOR SphericalToCartesian_(float radius, float theta, float phi) {
		return DirectX::XMVectorSet(
			radius * sinf(phi) * cosf(theta),
			radius * cosf(phi),
			radius * sinf(phi) * sinf(theta),
			1.0f);
	}

	static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M) {
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		DirectX::XMMATRIX A = M;
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

	static DirectX::XMFLOAT4X4 Identity4x4() {
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}

	static DirectX::XMVECTOR RandUnitVec3();
	static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

	static bool IsEqual(const math::Mat4& A, const math::Mat4& B);
	static bool IsEqual(const DirectX::XMMATRIX& A, const DirectX::XMMATRIX& B);
	static bool IsNearEqual(const DirectX::XMMATRIX& A, const DirectX::XMMATRIX& B, float epsilon);

	//static bool MathHelper::IsEqual(const math::Vec3& A, const math::Vec3& B) {
	//	return A.x == B.x && A.y == B.y && A.z == B.z;
	//}

	//static bool MathHelper::IsEqual(const math::Quat& A, const math::Quat& B) {
	//	return A.x == B.x && A.y == B.y && A.z == B.z && A.w == B.w;
	//}

	static constexpr float Infinity = FLT_MAX;
	static constexpr float Pi = 3.1415926535f;
};

