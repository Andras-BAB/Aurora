#pragma once

#include "MeshAsset.h"

namespace Aurora {
	class MeshInstance {
	public:
		MeshInstance() = default;
		virtual ~MeshInstance() = default;

		std::shared_ptr<MeshAsset> GetMesh();
		std::shared_ptr<MaterialAsset> GetMaterial(uint32_t index);

		static std::shared_ptr<MeshInstance> Create(const std::shared_ptr<MeshAsset>& meshAsset, 
			const std::vector<std::shared_ptr<MaterialAsset>>& materials);

	private:
		std::shared_ptr<MeshAsset> m_Mesh;
		std::vector<std::shared_ptr<MaterialAsset>> m_Materials;
	};
}
