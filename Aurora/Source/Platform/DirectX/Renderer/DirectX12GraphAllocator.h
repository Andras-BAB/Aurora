#pragma once

#include "Aurora/Renderer/RenderGraph.h"
#include "Platform/DirectX/Renderer/DirectX12Context.h"
#include "Platform/DirectX/Renderer/DirectX12HeapManager.h"
#include "Platform/DirectX/Renderer/DirectX12TextureManager.h"
#include "Platform/DirectX/Utils/d3dUtil.h"

#include "D3D12MemAlloc.h"

namespace Aurora {
	struct TexturePoolKey {
		uint32_t Width;
		uint32_t Height;
		ImageFormat Format;
		uint32_t SampleCount;

		bool operator==(const TexturePoolKey& other) const {
			return Width == other.Width && Height == other.Height && Format == other.Format && SampleCount == other.SampleCount;
		}
	};

	struct TexturePoolKeyHasher {
		size_t operator()(const TexturePoolKey& k) const {
			return std::hash<uint32_t>()(k.Width) ^
				(std::hash<uint32_t>()(k.Height) << 1) ^
				(std::hash<int>()(static_cast<int>(k.Format)) << 2) ^
				(std::hash<uint32_t>()(k.SampleCount) << 3);
		}
	};

	struct BufferPoolKey {
		uint32_t Size;
		bool operator==(const BufferPoolKey& other) const { return Size == other.Size; }
	};

	struct BufferPoolKeyHasher {
		size_t operator()(const BufferPoolKey& k) const { return std::hash<uint32_t>()(k.Size); }
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

		//struct PerFrameData {
		//	MS::ComPtr<D3D12MA::Allocation> BackingMemory;
		//	MS::ComPtr<D3D12MA::VirtualBlock> VirtualBlock;

		//	std::unordered_map<void*, D3D12MA::VirtualAllocation> ActiveAllocations;
		//	std::vector<RenderGraphResourceRegistry::PhysicalResourceData> FrameResources;

		//	std::vector<DescriptorRange> FrameRTVs;
		//	std::vector<DescriptorRange> FrameDSVs;
		//};

		//std::vector<PerFrameData> m_FrameData;

		//MS::ComPtr<D3D12MA::Allocation> m_BackingMemory;	// physical VRAM
		//MS::ComPtr<D3D12MA::VirtualBlock> m_VirtualBlock;	// mathematical memory manager

		//struct GraphResourceInfo {
		//	RenderGraphResourceRegistry::PhysicalResourceData Data;
		//	D3D12MA::VirtualAllocation VirtualAllocation;
		//};

		//std::unordered_map<void*, D3D12MA::VirtualAllocation> m_ActiveAllocations;

		//std::vector<RenderGraphResourceRegistry::PhysicalResourceData> m_FrameResources;

		//std::vector<DescriptorRange> m_FrameRTVs;
		//std::vector<DescriptorRange> m_FrameDSVs;

		std::unordered_map<TexturePoolKey, std::vector<RenderGraphResourceRegistry::PhysicalResourceData>, TexturePoolKeyHasher> m_FreeTextures;
		std::unordered_map<BufferPoolKey, std::vector<RenderGraphResourceRegistry::PhysicalResourceData>, BufferPoolKeyHasher> m_FreeBuffers;

		// connect the key to the physical data
		struct AllocatedTexture {
			TexturePoolKey Key;
			RenderGraphResourceRegistry::PhysicalResourceData Data;
		};
		std::vector<AllocatedTexture> m_AllocatedTextures;

		struct AllocatedBuffer {
			BufferPoolKey Key;
			RenderGraphResourceRegistry::PhysicalResourceData Data;
		};
		std::vector<AllocatedBuffer> m_AllocatedBuffers;

		std::unordered_map<void*, D3D12MA::Allocation*> m_MemoryAllocations;
	};
}
