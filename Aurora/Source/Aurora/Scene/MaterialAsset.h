#pragma once
#include "Aurora/Core/UUID.h"
#include "Aurora/Math/Math.h"
//#include "Aurora/Renderer/IRenderPipeline.h"

#include "Platform/DirectX/Renderer/DirectX12PipelineState.h"
#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {
	struct MaterialData {
		math::Vec4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		math::Vec3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		// TODO: fix MathHelper and use that
		//math::Mat4 MatTransform = MathHelper::Identity4x4();
		math::Mat4 MatTransform;

		uint32_t DiffuseMapIndex = 0xFFFFFFFF;

		std::string VertexShaderName = "baseVert";
		std::string PixelShaderName = "basePixel";

		BlendMode Blend = BlendMode::Opaque;
		DepthMode Depth = DepthMode::ReadWrite;
		CullMode Cull = CullMode::Back;
		bool Wireframe = false;
	};

	class MaterialAsset {
	public:
		MaterialAsset(const std::string& name);
		virtual ~MaterialAsset() = default;

		void SetDiffuseColor(const math::Vec4& color);
		void SetDiffuseMapIndex(uint32_t index);

		MaterialData& GetData() { return m_Data; }
		Aurora::UUID GetUUID() const { return m_Handle; }

		uint32_t GetVersion() const { return m_Version; }

		static std::shared_ptr<MaterialAsset> Create(const std::string& name, const MaterialData& materialData);

	private:
		Aurora::UUID m_Handle;
		MaterialData m_Data;
		uint32_t m_Version = 1;
	};
}
