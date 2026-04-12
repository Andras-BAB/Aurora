#pragma once

#include "MaterialAsset.h"

namespace Aurora {
	enum MaterialOverride : uint32_t {
		MaterialOverrideFlagNone = 0,
		DiffuseAlbedo		= 1 << 0,	// (0001)
		Roughness			= 1 << 1,	// (0010)
		DiffuseMapIndex		= 1 << 2,	// (0100)
		UVTransform			= 1 << 3,	// (1000)
		All = 0xFFFFFFFF
	};

	class MaterialInstance {
	public:
		MaterialInstance(const std::shared_ptr<MaterialAsset>& baseMaterial);
		~MaterialInstance() = default;

		MaterialData& GetData() { return m_OverrideData; }
		const MaterialData& GetData() const { return m_OverrideData; }

		std::shared_ptr<MaterialAsset> GetBaseMaterial() const { return m_BaseMaterial; }
		Aurora::UUID GetUUID() const { return m_InstanceID; }
		uint32_t GetVersion() const { return m_LocalVersion; }

		void SetDiffuseColor(const math::Vec4& color);
		//void SetDiffuseMapIndex(uint32_t index);
		void SetDiffuseMap(const std::shared_ptr<ITexture2D>& texture);
		void SetRoughness(float roughness);

		void ResetOverrides(MaterialOverride mask);
		void SetOverrides(uint32_t mask);
		bool HasOverride(MaterialOverride flag) const;

		MaterialData GetEffectiveData() const;
		uint32_t GetEffectiveVersion() const;

		static std::shared_ptr<MaterialInstance> Create(const std::shared_ptr<MaterialAsset>& baseMaterial);

	private:
		Aurora::UUID m_InstanceID;
		std::shared_ptr<MaterialAsset> m_BaseMaterial;

		MaterialData m_OverrideData;
		uint32_t m_OverrideMask = 0;

		uint32_t m_LocalVersion = 1;
	};
}
