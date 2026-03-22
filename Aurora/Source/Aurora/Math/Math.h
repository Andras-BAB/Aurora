#pragma once
#include "Config.h"

#if MATH_BACKEND == MATH_BACKEND_GLM
#include "Platform/glm/Vec2Glm.h"
#include "Platform/glm/Vec3Glm.h"
#include "Platform/glm/Vec4Glm.h"
#include "Platform/glm/Mat4Glm.h"
#elif MATH_BACKEND == MATH_BACKEND_DIRECTXMATH
#include "Platform/DirectX/Math/Vec2DX.h"
#include "Platform/DirectX/Math/Vec3DX.h"
#include "Platform/DirectX/Math/Vec4DX.h"
#include "Platform/DirectX/Math/Mat4DX.h"
#else
#error "Unknown math backend"
#endif
