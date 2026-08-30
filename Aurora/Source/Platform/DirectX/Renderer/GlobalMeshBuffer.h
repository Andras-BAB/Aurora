#pragma once
#include "Platform/DirectX/Utils/d3dUtil.h"

#include "D3D12MemAlloc.h"

namespace Aurora {
	struct MeshAllocation {
		uint64_t VertexOffsetBytes;		// byte offset
		uint32_t VertexSizeBytes;		// total size in bytes
		uint32_t VertexStride;			// bytes per vertex

		uint64_t IndexOffsetBytes;		// byte offset
		uint32_t IndexSizeBytes;		// total size in bytes
		uint32_t IndexCount;			// IndexSizeBytes / sizeof(uint32_t)

		D3D12MA::VirtualAllocation VertexVirtualAllocation;
		D3D12MA::VirtualAllocation IndexVirtualAllocation;
	};

	class FreeListAllocator {
	public:
		FreeListAllocator(uint32_t totalSize);

		uint32_t Allocate(uint32_t size);
		uint32_t Allocate(uint32_t size, uint32_t alignment);
		void Free(uint32_t offset, uint32_t size);
	private:
		struct FreeBlock {
			uint32_t offset;
			uint32_t size;
		};
		
		std::vector<FreeBlock> m_FreeBlocks;
	};

	struct MeshData {
		void*       VertexData;
		uint32_t    VertexSize;
		uint32_t    VertexStride;
		void*       IndexData;
		uint32_t    IndexSize;
	};

	class GlobalMeshBuffer {
	public:
		GlobalMeshBuffer(ID3D12Device* device, D3D12MA::Allocator* allocator, uint32_t vSize, uint32_t iSize);

		MeshAllocation AllocateMeshCPU(const MeshData& meshData);
		void UploadMeshToGPU(ID3D12GraphicsCommandList* cmdList,
			std::vector<MS::ComPtr<D3D12MA::Allocation>>& outStagingBuffers,
			const MeshData& meshData, const MeshAllocation& alloc) const;

		void FreeMesh(const MeshAllocation& alloc);

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
		ID3D12Resource* GetVertexResource() const;
		ID3D12Resource* GetIndexResource() const;

	private:
		MS::ComPtr<D3D12MA::Allocation> CreateUploadBuffer(void* data, uint32_t size) const;

	private:
		ID3D12Device* m_Device;
		D3D12MA::Allocator* m_Allocator;

		// only call IASetVertexBuffers, if the stride changed
		std::unique_ptr<d3dUtil::MeshGeometry> m_Geometry;

		MS::ComPtr<D3D12MA::Allocation> m_VertexAllocation;
		MS::ComPtr<D3D12MA::Allocation> m_IndexAllocation;

		MS::ComPtr<D3D12MA::VirtualBlock> m_VertexVirtualBlock;
		MS::ComPtr<D3D12MA::VirtualBlock> m_IndexVirtualBlock;

		//FreeListAllocator m_VertexAllocator;
		//FreeListAllocator m_IndexAllocator;
	};
}
