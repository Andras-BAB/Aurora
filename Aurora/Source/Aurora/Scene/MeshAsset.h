#pragma once

#include "MaterialAsset.h"
#include "Aurora/Core/UUID.h"

#include "Platform/DirectX/Renderer/GlobalMeshBuffer.h"
#include "Platform/DirectX/Math/Mat4DX.h"

namespace Aurora {
	struct SubmeshGeometry {
		uint32_t IndexCount;
		uint32_t StartIndexLocation;
		uint32_t BaseVertexLocation;

		// TODO: bounding box
	};
	
	struct SubmeshInstance {
		std::string Name;
		uint32_t SubmeshIndex;
		uint32_t MaterialIndex;
		//std::shared_ptr<MaterialAsset> Material;
		math::Mat4 LocalTransform;
	};

	class MeshAsset {
	public:
		MeshAsset(const std::string& filePath);
		MeshAsset(const std::string& name, const MeshData& meshData);
		virtual ~MeshAsset() = default;

		const MeshAllocation& GetAllocation() const { return m_Allocation; }
		Aurora::UUID GetUUID() const { return m_Handle; }

		const std::vector<SubmeshGeometry>& GetSubmeshes() const { return m_Submeshes; }
		const std::vector<SubmeshInstance>& GetSubmeshInstances() const { return m_SubmeshInstances; }

		std::shared_ptr<MaterialAsset> GetMaterialForSubmesh(uint32_t submeshIndex) const;

		static std::shared_ptr<MeshAsset> Create(const std::string& name, const MeshData& meshData);
		static std::shared_ptr<MeshAsset> LoadFromFile(const std::filesystem::path& path);

	private:
		Aurora::UUID m_Handle;
		//std::string m_Name;
		MeshAllocation m_Allocation;

		// ha egy fájlban több objektum van (pl. egy ház + ablakok)
		std::vector<SubmeshGeometry> m_Submeshes; // local X world location
		std::vector<SubmeshInstance> m_SubmeshInstances;
	};
	
}
