#include "aupch.h"
#include "GlobalMeshBuffer.h"

#include "FrameResource.h"

namespace Aurora {
	FreeListAllocator::FreeListAllocator(uint32_t totalSize) {
		m_FreeBlocks.push_back({ 0, totalSize });
	}

	uint32_t FreeListAllocator::Allocate(uint32_t size) {
		uint32_t alignedSize = (size + 255) & ~255;

		for (auto it = m_FreeBlocks.begin(); it != m_FreeBlocks.end(); ++it) {
			if (it->size >= alignedSize) {
				uint32_t offset = it->offset;

				if (it->size == alignedSize) {
					m_FreeBlocks.erase(it);
				} else {
					it->offset += alignedSize;
					it->size -= alignedSize;
				}
				return offset;
			}
		}
		return 0xFFFFFFFF; // no free space
	}

	uint32_t FreeListAllocator::Allocate(uint32_t size, uint32_t alignment) {
		//uint32_t alignmentMask = alignment - 1;

		for (auto it = m_FreeBlocks.begin(); it != m_FreeBlocks.end(); ++it) {
			//uint32_t padding = (alignment - (it->offset & alignmentMask)) & alignmentMask;
			//uint32_t totalNeeded = size + padding;
			uint32_t remainder = it->offset % alignment;
			uint32_t padding = (remainder == 0) ? 0 : (alignment - remainder);
			uint32_t totalNeeded = size + padding;

			if (it->size >= totalNeeded) {
				uint32_t allocationOffset = it->offset + padding;

				if (it->size == totalNeeded) {
					m_FreeBlocks.erase(it);
				} else {
					it->offset += totalNeeded;
					it->size -= totalNeeded;
				}
				return allocationOffset;
			}
		}
		return 0xFFFFFFFF;
	}

	void FreeListAllocator::Free(uint32_t offset, uint32_t size) {
		uint32_t alignedSize = (size + 255) & ~255;

		FreeBlock newBlock = { .offset = offset, .size = alignedSize };
		auto it = std::lower_bound(m_FreeBlocks.begin(), m_FreeBlocks.end(), newBlock,
			[](const FreeBlock& a, const FreeBlock& b) { return a.offset < b.offset; });

		auto insertedIt = m_FreeBlocks.insert(it, newBlock);

		auto next = std::next(insertedIt);
		if (next != m_FreeBlocks.end() && (insertedIt->offset + insertedIt->size == next->offset)) {
			insertedIt->size += next->size;
			m_FreeBlocks.erase(next);
		}

		if (insertedIt != m_FreeBlocks.begin()) {
			auto prev = std::prev(insertedIt);
			if (prev->offset + prev->size == insertedIt->offset) {
				prev->size += insertedIt->size;
				m_FreeBlocks.erase(insertedIt);
			}
		}
	}

	GlobalMeshBuffer::GlobalMeshBuffer(ID3D12Device* device, D3D12MA::Allocator* allocator, uint32_t vSize, uint32_t iSize) 
		: m_Device(device), m_Allocator(allocator)//, m_VertexAllocator(vSize), m_IndexAllocator(iSize) 
	{
		m_Geometry = std::make_unique<d3dUtil::MeshGeometry>();
		m_Geometry->Name = "GlobalMegaBuffer";
		m_Geometry->VertexBufferByteSize = vSize;
		m_Geometry->IndexBufferByteSize = iSize;
		m_Geometry->IndexFormat = DXGI_FORMAT_R32_UINT;
		// TODO: dynamic stride
		m_Geometry->VertexByteStride = sizeof(Vertex);

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC vDesc{};
		vDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vDesc.Alignment = 0;
		vDesc.Width = vSize;
		vDesc.Height = 1;
		vDesc.DepthOrArraySize = 1;
		vDesc.MipLevels = 1;
		vDesc.Format = DXGI_FORMAT_UNKNOWN;
		vDesc.SampleDesc.Count = 1;
		vDesc.SampleDesc.Quality = 0;
		vDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		vDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowOnFail(m_Allocator->CreateResource(
			&allocDesc,
			&vDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			m_VertexAllocation.GetAddressOf(),
			IID_PPV_ARGS(&m_Geometry->VertexBufferGPU)
		));

		D3D12_RESOURCE_DESC iDesc = vDesc;
		iDesc.Width = iSize;

		ThrowOnFail(m_Allocator->CreateResource(
			&allocDesc,
			&iDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			m_IndexAllocation.GetAddressOf(),
			IID_PPV_ARGS(&m_Geometry->IndexBufferGPU)
		));

		D3D12MA::VIRTUAL_BLOCK_DESC vertexBlockDesc = {};
		vertexBlockDesc.Size = vSize;
		ThrowOnFail(D3D12MA::CreateVirtualBlock(&vertexBlockDesc, m_VertexVirtualBlock.GetAddressOf()));

		D3D12MA::VIRTUAL_BLOCK_DESC indexBlockDesc = {};
		indexBlockDesc.Size = iSize;
		ThrowOnFail(D3D12MA::CreateVirtualBlock(&indexBlockDesc, m_IndexVirtualBlock.GetAddressOf()));
	}

	MeshAllocation GlobalMeshBuffer::AllocateMeshCPU(const MeshData& meshData) {
		if (meshData.VertexSize == 0 || meshData.IndexSize == 0) {
			AU_CORE_WARN("AllocateMeshCPU was called with an empty mesh! (VertexSize: {0}, IndexSize: {1})", meshData.VertexSize, meshData.IndexSize);
			return {};
		}

		MeshAllocation alloc = {};
		alloc.VertexSizeBytes = meshData.VertexSize;
		alloc.IndexSizeBytes = meshData.IndexSize;
		alloc.VertexStride = meshData.VertexStride;
		alloc.IndexCount = meshData.IndexSize / sizeof(uint32_t);

		D3D12MA::VIRTUAL_ALLOCATION_DESC vAllocDesc = {};
		vAllocDesc.Size = meshData.VertexSize;
		//vAllocDesc.Alignment = meshData.VertexStride;
		vAllocDesc.Alignment = 4;

		HRESULT hrV = m_VertexVirtualBlock->Allocate(&vAllocDesc, &alloc.VertexVirtualAllocation, &alloc.VertexOffsetBytes);
		
		D3D12MA::VIRTUAL_ALLOCATION_DESC iAllocDesc = {};
		iAllocDesc.Size = meshData.IndexSize;
		iAllocDesc.Alignment = sizeof(uint32_t); // TODO: dynamic index size

		HRESULT hrI = m_IndexVirtualBlock->Allocate(&iAllocDesc, &alloc.IndexVirtualAllocation, &alloc.IndexOffsetBytes);

		if (FAILED(hrV) || FAILED(hrI)) {
			AU_CORE_ERROR("Not enough space in GlobalMeshBuffers! Try to increase size or free up space!");
			// if only one succeeded, then release it
			if (SUCCEEDED(hrV)) m_VertexVirtualBlock->FreeAllocation(alloc.VertexVirtualAllocation);
			if (SUCCEEDED(hrI)) m_IndexVirtualBlock->FreeAllocation(alloc.IndexVirtualAllocation);
			return {};
		}

		return alloc;
	}

	void GlobalMeshBuffer::UploadMeshToGPU(ID3D12GraphicsCommandList* cmdList,
		std::vector<MS::ComPtr<D3D12MA::Allocation>>& outStagingBuffers, const MeshData& meshData,
		const MeshAllocation& alloc) const {

		MS::ComPtr<D3D12MA::Allocation> vUploader = CreateUploadBuffer(meshData.VertexData, meshData.VertexSize);
		MS::ComPtr<D3D12MA::Allocation> iUploader = CreateUploadBuffer(meshData.IndexData, meshData.IndexSize);

		cmdList->CopyBufferRegion(m_Geometry->VertexBufferGPU.Get(), alloc.VertexOffsetBytes, vUploader->GetResource(), 0, meshData.VertexSize);
		cmdList->CopyBufferRegion(m_Geometry->IndexBufferGPU.Get(), alloc.IndexOffsetBytes, iUploader->GetResource(), 0, meshData.IndexSize);

		outStagingBuffers.push_back(vUploader);
		outStagingBuffers.push_back(iUploader);
	}

	void GlobalMeshBuffer::FreeMesh(const MeshAllocation& alloc) {
		if (alloc.VertexVirtualAllocation.AllocHandle) {
			m_VertexVirtualBlock->FreeAllocation(alloc.VertexVirtualAllocation);
		}
		if (alloc.IndexVirtualAllocation.AllocHandle) {
			m_IndexVirtualBlock->FreeAllocation(alloc.IndexVirtualAllocation);
		}
	}

	D3D12_VERTEX_BUFFER_VIEW GlobalMeshBuffer::GetVertexBufferView() const {
		return m_Geometry->VertexBufferView();
	}

	D3D12_INDEX_BUFFER_VIEW GlobalMeshBuffer::GetIndexBufferView() const {
		return m_Geometry->IndexBufferView();
	}

	ID3D12Resource* GlobalMeshBuffer::GetVertexResource() const {
		return m_Geometry->VertexBufferGPU.Get();
	}

	ID3D12Resource* GlobalMeshBuffer::GetIndexResource() const {
		return m_Geometry->IndexBufferGPU.Get();
	}

	MS::ComPtr<D3D12MA::Allocation> GlobalMeshBuffer::CreateUploadBuffer(void* data, uint32_t size) const {
		MS::ComPtr<D3D12MA::Allocation> allocation;
		MS::ComPtr<ID3D12Resource> uploader;

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_HEAP_PROPERTIES uploadProps{};
		uploadProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = static_cast<UINT64>(size);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowOnFail(m_Allocator->CreateResource(&allocDesc, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			allocation.GetAddressOf(), IID_PPV_ARGS(&uploader)));

		if (data) {
			void* mapped = nullptr;
			uploader->Map(0, nullptr, &mapped);
			memcpy(mapped, data, size);
			uploader->Unmap(0, nullptr);
		}

		return allocation;
	}
}
