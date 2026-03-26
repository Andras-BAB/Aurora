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
	
	MeshAsset::MeshAsset(const std::string& name, const MeshData& meshData) {
		// TODO: use name
		m_Handle = UUID();
		m_Allocation = RenderCommand::AllocateMesh(meshData);
	}

	std::shared_ptr<MeshAsset> MeshAsset::Create(const std::string& name, const MeshData& meshData) {
		auto meshAsset = std::make_shared<MeshAsset>(name, meshData);
		Application::Get().GetAssetRegistry().AddMesh(meshAsset);
		return meshAsset;
	}
}
