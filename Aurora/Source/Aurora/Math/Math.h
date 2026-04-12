#pragma once
#include "Config.h"

#include "Aurora/Math/Vec2.h"
#include "Aurora/Math/Vec3.h"
#include "Aurora/Math/Vec4.h"
#include "Aurora/Math/Mat4.h"
#include "Aurora/Math/Quat.h"

namespace math {
	static constexpr float PI = 3.1415926535f;

	inline float Radians(float degrees) {
		return degrees * (PI / 180.0f);
	}
}
