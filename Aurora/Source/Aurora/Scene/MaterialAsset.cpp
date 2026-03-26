#include "aupch.h"

#include "MaterialAsset.h"

#include "Aurora/Core/Application.h"

namespace Aurora {
	MaterialAsset::MaterialAsset(const std::string& name) {
		m_Handle = UUID();
	}

	void MaterialAsset::SetDiffuseColor(const math::Vec4& color) {
		m_Data.DiffuseAlbedo = color;
		m_Version++;
	}

	void MaterialAsset::SetDiffuseMapIndex(uint32_t index) {
		m_Data.DiffuseMapIndex = index;
		m_Version++;
	}

	std::shared_ptr<MaterialAsset> MaterialAsset::Create(const std::string& name, const MaterialData& materialData) {
		auto asset = std::make_shared<MaterialAsset>(name);

		//auto& data = asset->GetData();
		//data.DiffuseAlbedo = materialData.DiffuseAlbedo;
		//data.FresnelR0 = materialData.FresnelR0;
		//data.Roughness = materialData.Roughness;
		//data.MatTransform = materialData.MatTransform;

		asset->GetData() = materialData;
		//data.Wireframe = true;

		Application::Get().GetAssetRegistry().AddMaterial(asset);
		return asset;
	}
}
