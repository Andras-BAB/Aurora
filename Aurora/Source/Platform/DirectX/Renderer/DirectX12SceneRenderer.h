#pragma once
#include "Aurora/Renderer/SceneRenderer.h"

namespace Aurora {
	class DirectX12SceneRenderer : public SceneRenderer {
	public:
		DirectX12SceneRenderer() = default;
		~DirectX12SceneRenderer() override = default;

		void Render(Scene* scene) override;

	private:
		//PassConstants
	};
}
