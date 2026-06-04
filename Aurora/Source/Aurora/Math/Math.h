#pragma once
#include "Config.h"

#include "Aurora/Math/Vec2.h"
#include "Aurora/Math/Vec3.h"
#include "Aurora/Math/Vec4.h"
#include "Aurora/Math/Mat4.h"
#include "Aurora/Math/Quat.h"

namespace math {
	static constexpr float PI = 3.1415926535f;
	static constexpr float TWO_PI = 6.283185307f;
	static constexpr float ONE_DIVPI = 0.318309886f;
	static constexpr float ONE_DIV2PI = 0.159154943f;
	static constexpr float PIDIV2 = 1.570796327f;
	static constexpr float PIDIV4 = 0.785398163f;

	inline float Radians(float degrees) {
		return degrees * (PI / 180.0f);
	}
}
