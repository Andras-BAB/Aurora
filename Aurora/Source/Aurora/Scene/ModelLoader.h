#pragma once

#include "Prefab.h"

namespace Aurora {
	class ModelLoader {
	public:
		static std::shared_ptr<Prefab> Load(const std::filesystem::path& path);
	};
}
