#include "aupch.h"

#include "MaterialInstance.h"

namespace Aurora {

	std::shared_ptr<MaterialInstance> MaterialInstance::Create(const std::shared_ptr<MaterialAsset>& baseMaterial) {
		return std::make_shared<MaterialInstance>(baseMaterial);
	}

	MaterialInstance::MaterialInstance(const std::shared_ptr<MaterialAsset>& baseMaterial) : m_BaseMaterial(baseMaterial) {

	}

	void MaterialInstance::SetDiffuseColor(const math::Vec4& color) {
		m_OverrideData.DiffuseAlbedo = color;
		m_OverrideMask |= MaterialOverride::DiffuseAlbedo;
		m_LocalVersion++;
	}

	//void MaterialInstance::SetDiffuseMapIndex(uint32_t index) {
	//	//m_OverrideData.DiffuseMapIndex = index;
	//	m_OverrideData.DiffuseMap->SetHandle({ index });
	//	m_OverrideMask |= MaterialOverride::DiffuseMapIndex;
	//	m_LocalVersion++;
	//}

	void MaterialInstance::SetDiffuseMap(const std::shared_ptr<ITexture2D>& texture) {
		m_OverrideData.DiffuseMap = texture;
		m_OverrideMask |= MaterialOverride::DiffuseMapIndex;
		m_LocalVersion++;
	}

	void MaterialInstance::SetRoughness(float roughness) {
		m_OverrideData.Roughness = roughness;
		m_OverrideMask |= MaterialOverride::Roughness;
		m_LocalVersion++;
	}

	void MaterialInstance::ResetOverrides(MaterialOverride mask) {
		m_OverrideMask &= ~mask;
		m_LocalVersion++;
	}

	void MaterialInstance::SetOverrides(uint32_t mask) {
		m_OverrideMask |= mask;
		m_LocalVersion++;
	}

	bool MaterialInstance::HasOverride(MaterialOverride flag) const {
		return (m_OverrideMask & flag) != 0;
	}

	MaterialData MaterialInstance::GetEffectiveData() const {
		MaterialData result = m_BaseMaterial->GetDefaultData();

		if (HasOverride(MaterialOverride::DiffuseAlbedo))	result.DiffuseAlbedo = m_OverrideData.DiffuseAlbedo;
		if (HasOverride(MaterialOverride::Roughness))		result.Roughness = m_OverrideData.Roughness;
		if (HasOverride(MaterialOverride::DiffuseMapIndex)) result.DiffuseMap = m_OverrideData.DiffuseMap;
		if (HasOverride(MaterialOverride::UVTransform))		result.UVTransform = m_OverrideData.UVTransform;

		return result;
	}

	uint32_t MaterialInstance::GetEffectiveVersion() const {
		// TODO: do a proper hashing
		return m_LocalVersion + (m_BaseMaterial->GetVersion() * 137);
	}
}
