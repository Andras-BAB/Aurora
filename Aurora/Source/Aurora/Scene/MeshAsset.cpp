#include "aupch.h"

#include "MeshAsset.h"

#include "Aurora/Core/Application.h"
#include "Platform/DirectX/Renderer/FrameResource.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "Platform/DirectX/Utils/GeometryGenerator.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Aurora {
	
	MeshAsset::MeshAsset(const std::string& name, const MeshData& meshData, Aurora::UUID uuid) {
		// TODO: use name
		m_Handle = uuid;
		m_Allocation = RenderCommand::AllocateMesh(meshData);
	}

	MeshAsset::~MeshAsset() {
		if (m_Allocation.VertexVirtualAllocation.AllocHandle || m_Allocation.IndexVirtualAllocation.AllocHandle) {
			RenderCommand::FreeMesh(m_Allocation);
		}
	}

	std::shared_ptr<MeshAsset> MeshAsset::Create(const std::string& name, const MeshData& meshData, Aurora::UUID uuid) {
		auto meshAsset = std::make_shared<MeshAsset>(name, meshData, uuid);
		Application::Get().GetAssetRegistry().AddMesh(meshAsset);
		return meshAsset;
	}
}
