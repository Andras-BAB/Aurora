#pragma once

#include "Platform/DirectX/Renderer/DirectX12GraphAllocator.h"

namespace Aurora {
	class Scene;

	class SceneRenderer {
	public:
		SceneRenderer() = default;
		virtual ~SceneRenderer() = default;

		// returns the final image texture handle
		virtual TextureHandle Render(Scene* scene, float viewportWidth = 0, float viewportHeight = 0);

	private:
		std::unique_ptr<DirectX12GraphAllocator> m_GraphAllocator;
		std::unique_ptr<FrameAllocator> m_FrameAllocator;
	};
}
