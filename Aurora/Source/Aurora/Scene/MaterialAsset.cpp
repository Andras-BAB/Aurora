#include "aupch.h"

#include "MaterialAsset.h"

#include "Aurora/Core/Application.h"

namespace Aurora {
	MaterialAsset::MaterialAsset(const std::string& name) {
		m_Data.Name = name;
		m_Handle = UUID();
	}

	void MaterialAsset::SetDiffuseColor(const math::Vec4& color) {
		//m_Data.DiffuseAlbedo = color;
		DirectX::XMStoreFloat4(&m_Data.DiffuseAlbedo, color);
		// TODO: make it dynamic
		m_Data.NumFramesDirty = 3;
	}

	std::shared_ptr<MaterialAsset> MaterialAsset::Create(const std::string& name, const MaterialData& materialData) {
		auto asset = std::make_shared<MaterialAsset>(name);

		auto& data = asset->GetData();
		//data.DiffuseAlbedo = materialData.DiffuseAlbedo;
		DirectX::XMStoreFloat4(&data.DiffuseAlbedo, materialData.DiffuseAlbedo);
		//data.FresnelR0 = materialData.FresnelR0;
		DirectX::XMStoreFloat3(&data.FresnelR0, materialData.FresnelR0);
		data.Roughness = materialData.Roughness;
		data.MatTransform = materialData.MatTransform;
		// TODO: make it dynamic
		data.NumFramesDirty = 3;

		Application::Get().GetAssetRegistry().AddMaterial(asset);
		return asset;
	}
}
