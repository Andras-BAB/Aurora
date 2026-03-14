#pragma once

#include "MeshAsset.h"
#include "Platform/DirectX/Math/Vec3DX.h"

namespace Aurora {
	struct ModelNode {
		std::string Name;

		math::Vec3 Translation = { 0.0f, 0.0f, 0.0f };
		math::Vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		math::Vec3 Scale = { 1.0f, 1.0f, 1.0f };

		std::shared_ptr<MeshAsset> Mesh = nullptr;
		std::vector<std::shared_ptr<MaterialAsset>> Materials;

		std::vector<ModelNode> Children;
	};

	class Prefab {
	public:
		Prefab() = default;
		~Prefab() = default;

		ModelNode RootNode;
		Aurora::UUID GetUUID() const { return m_Handle; }

	private:
		Aurora::UUID m_Handle;
	};
}
