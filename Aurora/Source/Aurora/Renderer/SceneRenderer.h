#pragma once

#include "Platform/DirectX/Renderer/DirectX12GraphAllocator.h"

namespace Aurora {
	class Scene;

	class SceneRenderer {
	public:
		SceneRenderer() = default;
		virtual ~SceneRenderer() = default;

		virtual void Render(Scene* scene);

	private:
		std::unique_ptr<DirectX12GraphAllocator> m_GraphAllocator;
		std::unique_ptr<FrameAllocator> m_FrameAllocator;
	};
}
