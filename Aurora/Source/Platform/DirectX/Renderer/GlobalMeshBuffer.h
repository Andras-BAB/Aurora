#pragma once
#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {
	struct MeshAllocation {
		uint32_t VertexOffsetBytes;		// byte offset
		uint32_t VertexSizeBytes;		// total size in bytes
		uint32_t VertexStride;			// bytes per vertex

		uint32_t IndexOffsetBytes;		// byte offset
		uint32_t IndexSizeBytes;		// total size in bytes
		uint32_t IndexCount;			// IndexSizeBytes / sizeof(uint32_t)
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
		GlobalMeshBuffer(ID3D12Device* device, uint32_t vSize, uint32_t iSize);

		MeshAllocation AllocateMeshCPU(const MeshData& meshData);
		void UploadMeshToGPU(ID3D12GraphicsCommandList* cmdList,
			std::vector<MS::ComPtr<ID3D12Resource>>& outStagingBuffers,
			const MeshData& meshData, const MeshAllocation& alloc) const;

		//MeshAllocation AllocateMesh(ID3D12GraphicsCommandList* cmdList,
		//	std::vector<MS::ComPtr<ID3D12Resource>>& outStagingBuffers,
		//	//void* vData, uint32_t vSize, uint32_t vStride,
		//	//void* iData, uint32_t iSize);
		//	const MeshData& meshData);
		void FreeMesh(const MeshAllocation& alloc);

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
		ID3D12Resource* GetVertexResource() const;
		ID3D12Resource* GetIndexResource() const;

	private:
		MS::ComPtr<ID3D12Resource> CreateUploadBuffer(void* data, uint32_t size) const;

	private:
		ID3D12Device* m_Device;
		// érdemes lehet külön szedni stride szerint és külön bufferekbe rakni.
		// például gyorsabb depth-only passhoz külön a pozíciót, mert más annak nem kell, a többi meg egybe egy másik bufferbe

		// addig most:
		// A GlobalMeshBuffer tárolja el a VertexStride-ot minden foglalásnál.
		// A DrawRenderItems ciklusban figyeld, hogy az aktuális mesh stride - ja megegyezik - e az előzőével.
		// Csak akkor hívj IASetVertexBuffers - t, ha a stride megváltozott.
		std::unique_ptr<d3dUtil::MeshGeometry> m_Geometry;

		//MS::ComPtr<ID3DBlob> m_VertexBufferCPU;
		//MS::ComPtr<ID3DBlob> m_IndexBufferCPU;

		//MS::ComPtr<ID3D12Resource> m_VertexBufferGPU;
		//MS::ComPtr<ID3D12Resource> m_IndexBufferGPU;
				
		FreeListAllocator m_VertexAllocator;
		FreeListAllocator m_IndexAllocator;
	};
}
