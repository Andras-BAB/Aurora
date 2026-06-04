#pragma once

#include "Aurora/Math/Vec3.h"

namespace Aurora {
	struct ClusterData {
		uint32_t Offset;
		uint32_t Count;
	};
	// StructuredBuffer::Create(GridSizeX * GridSizeY * GridSizeZ, sizeof(ClusterData))

	struct PointLight {
		math::Vec3 PositionW;
		float Radius;
		math::Vec3 Color;
		float Intensity;
	};

	struct SpotLight {
		math::Vec3 PositionW;
		float Radius;
		math::Vec3 Direction;
		float Angle;
		math::Vec3 Color;
		float Padding;
	};

	struct DirectionalLight {
		math::Vec3 Direction;
		float Padding1;
		math::Vec3 Strength;
		float Padding2;
	};
}
