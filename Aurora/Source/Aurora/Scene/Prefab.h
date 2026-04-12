#pragma once

#include "MeshAsset.h"
#include "MaterialAsset.h"

#include "Aurora/Math/Vec3.h"

namespace Aurora {
	struct ModelNode {
		std::string Name;

		math::Vec3 Translation = { 0.0f, 0.0f, 0.0f };
		//math::Vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		math::Quat Rotation = math::Quat::Identity();
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
		void SetUUID(Aurora::UUID uuid) { m_Handle = uuid; }

	private:
		Aurora::UUID m_Handle;
	};
}
