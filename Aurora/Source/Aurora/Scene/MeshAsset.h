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
		math::Mat4 LocalTransform;
	};

	class MeshAsset {
	public:
		MeshAsset(const std::string& name, const MeshData& meshData);
		virtual ~MeshAsset() = default;

		const MeshAllocation& GetAllocation() const { return m_Allocation; }
		Aurora::UUID GetUUID() const { return m_Handle; }

		const std::vector<SubmeshGeometry>& GetSubmeshes() const { return m_Submeshes; }
		const std::vector<SubmeshInstance>& GetSubmeshInstances() const { return m_SubmeshInstances; }

		static std::shared_ptr<MeshAsset> Create(const std::string& name, const MeshData& meshData);

	private:
		Aurora::UUID m_Handle;
		//std::string m_Name;
		MeshAllocation m_Allocation;

		std::vector<SubmeshGeometry> m_Submeshes; // local X world location
		std::vector<SubmeshInstance> m_SubmeshInstances;

		friend class ModelLoader;
	};
	
}
