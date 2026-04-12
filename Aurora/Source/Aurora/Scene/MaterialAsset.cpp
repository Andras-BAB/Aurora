#include "aupch.h"

#include "MaterialAsset.h"

#include "Aurora/Core/Application.h"

namespace Aurora {
	MaterialAsset::MaterialAsset(const std::string& name, Aurora::UUID uuid) {
		m_Handle = uuid;
	}

	void MaterialAsset::SetDiffuseColor(const math::Vec4& color) {
		m_DefaultData.DiffuseAlbedo = color;
		m_Version++;
	}

	void MaterialAsset::SetDiffuseMap(const std::shared_ptr<ITexture2D>& texture) {
		m_DefaultData.DiffuseMap = texture;
		m_Version++;
	}

	//void MaterialAsset::SetDiffuseMapIndex(uint32_t index) {
	//	m_DefaultData.DiffuseMap->SetHandle({ index });
	//	m_Version++;
	//}

	std::shared_ptr<MaterialAsset> MaterialAsset::Create(const std::string& name, const MaterialData& materialData, Aurora::UUID uuid) {
		auto asset = std::make_shared<MaterialAsset>(name, uuid);

		asset->GetDefaultData() = materialData;

		Application::Get().GetAssetRegistry().AddMaterial(asset);
		return asset;
	}
}
