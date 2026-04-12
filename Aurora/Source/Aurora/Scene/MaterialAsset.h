#pragma once

#include "Aurora/Core/UUID.h"
#include "Aurora/Math/Math.h"
//#include "Aurora/Renderer/IRenderPipeline.h"

#include "Aurora/Renderer/Texture.h"

#include "Platform/DirectX/Renderer/DirectX12PipelineState.h"
#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {
	struct MaterialData {
		math::Vec4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		math::Vec3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;

		math::Vec4 UVTransform = { 1.0f, 1.0f, 0.0f, 0.0f }; // scaleX, scaleY, offsetX, offsetY for tiling and texture animation

		//uint32_t DiffuseMapIndex = 0xFFFFFFFF;

		std::shared_ptr<ITexture2D> DiffuseMap = nullptr;

		uint32_t GetDiffuseMapIndex() const {
			return DiffuseMap ? DiffuseMap->GetHandle().Index : 0xFFFFFFFF;
		}
	};

	struct MaterialPipelineData {
		// TODO: use UUID instead of string
		std::string VertexShaderName = "baseVert";
		std::string PixelShaderName = "basePixel";

		BlendMode Blend = BlendMode::Opaque;
		DepthMode Depth = DepthMode::ReadWrite;
		CullMode Cull = CullMode::Back;
		bool Wireframe = false;
	};

	class MaterialAsset {
	public:
		MaterialAsset(const std::string& name, Aurora::UUID uuid);
		virtual ~MaterialAsset() = default;

		void SetDiffuseColor(const math::Vec4& color);
		//void SetDiffuseMapIndex(uint32_t index);
		void SetDiffuseMap(const std::shared_ptr<ITexture2D>& texture);

		const MaterialPipelineData& GetPipelineData() const { return m_PipelineData; }
		MaterialData& GetDefaultData() { return m_DefaultData; }
		Aurora::UUID GetUUID() const { return m_Handle; }

		uint32_t GetVersion() const { return m_Version; }

		static std::shared_ptr<MaterialAsset> Create(const std::string& name, const MaterialData& materialData, Aurora::UUID uuid = Aurora::UUID());

	private:
		Aurora::UUID m_Handle;
		MaterialPipelineData m_PipelineData;
		MaterialData m_DefaultData;
		uint32_t m_Version = 1;
	};
}
