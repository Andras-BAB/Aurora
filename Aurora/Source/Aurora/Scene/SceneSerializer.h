#pragma once

#include "Scene.h"

namespace Aurora {
	class SceneSerializer {
	public:
		SceneSerializer(const std::shared_ptr<Scene>& scene);
		~SceneSerializer() = default;

		bool Serialize(const std::string& path);
		bool Deserialize(const std::string& path);

	private:
		std::shared_ptr<Scene> m_Scene;
	};
}
