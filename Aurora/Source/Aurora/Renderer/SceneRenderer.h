#pragma once

namespace Aurora {
	class Scene;

	class SceneRenderer {
	public:
		SceneRenderer() = default;
		virtual ~SceneRenderer() = default;

		virtual void Render(Scene* scene);
	};
}
