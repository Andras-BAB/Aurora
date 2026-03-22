#pragma once
#include "Aurora/Core/UUID.h"
#include "Aurora/Math/Math.h"
#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {
	struct MaterialData {
		math::Vec4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		math::Vec3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		// TODO: fix MathHelper and use that
		//math::Mat4 MatTransform = MathHelper::Identity4x4();
		math::Mat4 MatTransform;
	};

	class MaterialAsset {
	public:
		MaterialAsset(const std::string& name);
		virtual ~MaterialAsset() = default;

		void SetDiffuseColor(const math::Vec4& color);

		d3dUtil::Material& GetData() { return m_Data; }
		Aurora::UUID GetUUID() const { return m_Handle; }

		static std::shared_ptr<MaterialAsset> Create(const std::string& name, const MaterialData& materialData);

	private:
		Aurora::UUID m_Handle;
		// TODO: do not use d3dUtil's material
		d3dUtil::Material m_Data;
	};
}
