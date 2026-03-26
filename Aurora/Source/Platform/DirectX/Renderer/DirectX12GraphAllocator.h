#pragma once

#include "Aurora/Renderer/RenderGraph.h"
#include "Platform/DirectX/Renderer/DirectX12Context.h"
#include "Platform/DirectX/Renderer/DirectX12HeapManager.h"
#include "Platform/DirectX/Renderer/DirectX12TextureManager.h"
#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {
	struct TexturePoolKey {
		uint32_t Width;
		uint32_t Height;
		ImageFormat Format;

		bool operator==(const TexturePoolKey& other) const {
			return Width == other.Width && Height == other.Height && Format == other.Format;
		}
	};

	struct TexturePoolKeyHasher {
		std::size_t operator()(const TexturePoolKey& k) const {
			return std::hash<uint32_t>()(k.Width) ^
				(std::hash<uint32_t>()(k.Height) << 1) ^
				(std::hash<int>()(static_cast<int>(k.Format)) << 2);
		}
	};

	class DirectX12GraphAllocator : public IRenderGraphAllocator {
	public:
		DirectX12GraphAllocator(DirectX12Context* context, DirectX12TextureManager* textureManager);

		~DirectX12GraphAllocator() override;

		//void AllocateTexture(const ResourceNode& node, RenderGraphResourceRegistry::PhysicalResourceData& outData) override;
		//void AllocateBuffer(const ResourceNode& node, RenderGraphResourceRegistry::PhysicalResourceData& outData) override;
		//void UpdateFinalState(const ResourceNode& node, uint32_t finalState) override;

		void AcquireTexture(const GraphTextureDesc& desc, RenderGraphResourceRegistry::PhysicalResourceData& outData) override;
		void ReleaseTexture(const GraphTextureDesc& desc, const RenderGraphResourceRegistry::PhysicalResourceData& data) override;
		void AcquireBuffer(const GraphBufferDesc& desc, RenderGraphResourceRegistry::PhysicalResourceData& outData) override;
		void ReleaseBuffer(const GraphBufferDesc& desc, const RenderGraphResourceRegistry::PhysicalResourceData& data) override;

		// reuse resources from previous frames
		void BeginFrame();
		void ClearPool();
		void UpdateFinalState(void* physicalResource, uint32_t finalState) override;

	private:
		DirectX12Context* m_Context;
		DirectX12TextureManager* m_TextureManager;

		std::unordered_map<TexturePoolKey, std::vector<RenderGraphResourceRegistry::PhysicalResourceData>, TexturePoolKeyHasher> m_FreeTextures;

		// connect the key to the physical data
		struct AllocatedTexture {
			TexturePoolKey Key;
			RenderGraphResourceRegistry::PhysicalResourceData Data;
		};

		std::vector<AllocatedTexture> m_AllocatedTextures;
	};
}
