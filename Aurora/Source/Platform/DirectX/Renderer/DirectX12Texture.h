#pragma once

#include "Aurora/Renderer/Texture.h"
#include "Platform/DirectX/Utils/MSUtils.h"
#include <d3d12.h>

namespace Aurora {
	class DirectX12Texture2D : public ITexture2D {
	public:
		DirectX12Texture2D(const TextureSpecification& specification);
		DirectX12Texture2D(const std::string& path);
		virtual ~DirectX12Texture2D();

		const TextureSpecification& GetSpecification() const override { return m_Specification; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		const std::string& GetPath() const override { return m_Path; }

		void SetData(void* data, uint32_t size) override;
		bool IsLoaded() const override { return m_IsLoaded; }

		TextureHandle GetHandle() const override { return m_Handle; }

		bool operator==(const ITexture& other) const override {
			return m_Handle == other.GetHandle();
		}

		ID3D12Resource* GetResource() const { return m_TextureResource.Get(); }

	private:
		TextureSpecification m_Specification;
		std::string m_Path;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		bool m_IsLoaded = false;

		TextureHandle m_Handle;

		MS::ComPtr<ID3D12Resource> m_TextureResource;
	};
}