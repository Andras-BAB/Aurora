#pragma once

#include "Aurora/Math/Math.h"

namespace Aurora {
	
	class Camera {
	public:
		Camera() = default;
		Camera(const math::Mat4& projection) : m_ProjectionMatrix(projection) {}
		virtual ~Camera() = default;

		const math::Mat4& GetProjection() const { return m_ProjectionMatrix; }
	
	protected:
		math::Mat4 m_ProjectionMatrix{};
	};
	
}
