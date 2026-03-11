//***************************************************************************************
// MathHelper.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "stdafx.h"
#include "Utils/MathHelper.h"
#include <cfloat>
#include <cmath>

using namespace DirectX;

float MathHelper::AngleFromXY(float x, float y) {
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f) {
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * Pi; // in [0, 2*pi).
	} else { // Quadrant II or III
		theta = atanf(y / x) + Pi; // in [0, 2*pi).
	}

	return theta;
}

XMVECTOR MathHelper::RandUnitVec3() {
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true) {
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR MathHelper::RandHemisphereUnitVec3(XMVECTOR n) {
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true) {
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// Ignore points in the bottom hemisphere.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}

bool MathHelper::IsEqual(const XMMATRIX& A, const XMMATRIX& B) {
	return XMVector4Equal(A.r[0], B.r[0]) &&
		XMVector4Equal(A.r[1], B.r[1]) &&
		XMVector4Equal(A.r[2], B.r[2]) &&
		XMVector4Equal(A.r[3], B.r[3]);
}


bool MathHelper::IsNearEqual(const DirectX::XMMATRIX& A, const DirectX::XMMATRIX& B, float epsilon) {
	XMVECTOR E = XMVectorReplicate(epsilon); 
	return XMVector4NearEqual(A.r[0], B.r[0], E) && 
		XMVector4NearEqual(A.r[1], B.r[1], E) && 
		XMVector4NearEqual(A.r[2], B.r[2], E) && 
		XMVector4NearEqual(A.r[3], B.r[3], E);
}
