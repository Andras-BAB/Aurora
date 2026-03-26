#pragma once
#include "Platform/DirectX/Utils/MSUtils.h"

#include <mutex>

namespace Aurora {
	struct DescriptorHandleCPU { D3D12_CPU_DESCRIPTOR_HANDLE handle{}; };
	struct DescriptorHandleGPU { D3D12_GPU_DESCRIPTOR_HANDLE handle{}; };

	struct DescriptorRange {
		DescriptorHandleCPU cpuBase{};
		DescriptorHandleGPU gpuBase{}; // for only shader-visible heap
		UINT count = 0;
	};

	enum class HeapType : uint8_t {
		RTV, 
		DSV, 
		CBV_SRV_UAV
	};

	struct DeferTicket { UINT64 fenceValue = 0; };

	class DescriptorPage {
	public:
		DescriptorPage(ID3D12Device* device,
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			bool shaderVisible,
			UINT capacity);

		bool Allocate(UINT count, DescriptorRange& out);
		void FreeDeferred(const DescriptorRange& range, UINT64 fenceValue);
		void ReleaseCompleted(UINT64 fenceCompleted);

		ID3D12DescriptorHeap* GetHeap() const { return m_Heap.Get(); }
		UINT GetCapacity() const { return m_Capacity; }
		UINT GetDescriptorSize() const { return m_DescriptorSize; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuStart() const { return m_Heap->GetCPUDescriptorHandleForHeapStart(); }
		bool IsShaderVisible() const { return m_ShaderVisible; }

	private:
		struct Pending { UINT start, len; UINT64 fence; };

		MS::ComPtr<ID3D12DescriptorHeap> m_Heap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_Type{};
		UINT m_DescriptorSize = 0;
		bool m_ShaderVisible = false;
		UINT m_Capacity = 0;

		std::vector<std::pair<UINT, UINT>> m_FreeRanges;
		std::vector<Pending> m_PendingFrees;
		mutable std::mutex m_Mutex;
	};

	class DescriptorAllocator {
	public:
		DescriptorAllocator(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type,
			bool shaderVisible, UINT pageSize);

		DescriptorRange Allocate(UINT count);                           // persistent pool
		void Free(const DescriptorRange& range, DeferTicket ticket);    // deferred free
		void ReleaseCompleted(UINT64 fenceCompleted);                   // free pending ranges

		ID3D12DescriptorHeap* GetPageForRange(const DescriptorRange& range) const;
		ID3D12DescriptorHeap* GetFirstPageHeap() const;

	private:
		ID3D12Device* m_Device = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE m_Type{};
		bool m_ShaderVisible = false;
		UINT m_PageSize = 0;
		std::vector<std::unique_ptr<DescriptorPage>> m_Pages;
	};

	class FrameDescriptorHeap {
	public:
		FrameDescriptorHeap(ID3D12Device* device, UINT pageSize);

		void BeginFrame(UINT64 fenceCompleted);
		DescriptorRange AllocateTransient(UINT count);					// linear allocation
		ID3D12DescriptorHeap* GetCurrentHeap() const;
		void EndFrame(UINT64 fenceSignal);								// whole heap deferred recycle

	private:
		// more heap page and write offset
		struct FramePage {
			MS::ComPtr<ID3D12DescriptorHeap> heap;
			UINT descriptorSize = 0;
			UINT capacity = 0;
			UINT offset = 0;
			UINT64 fenceSignal = 0;
		};

		void AllocateNewPage();
		void SelectActivePage();

		ID3D12Device* m_Device = nullptr;
		UINT m_PageSize = 0;
		std::vector<FramePage> m_Pages;
		UINT m_Active = 0;
	};


	class DirectX12HeapManager {
	public:
		DirectX12HeapManager(ID3D12Device* device,
			UINT rtvPageSize,
			UINT dsvPageSize,
			UINT cbvSrvUavPersistentPageSize,
			UINT cbvSrvUavFramePageSize);

		// Persistent allocations
		DescriptorRange AllocateRTV(UINT count = 1);
		void FreeRTV(const DescriptorRange& range, DeferTicket ticket);

		DescriptorRange AllocateDSV(UINT count = 1);
		void FreeDSV(const DescriptorRange& range, DeferTicket ticket);

		DescriptorRange AllocateCBV_SRV_UAV_Persistent(UINT count);
		void FreeCBV_SRV_UAV_Persistent(const DescriptorRange& range, DeferTicket ticket);

		UINT AllocateObjCBIndex();
		void FreeObjCBIndex(UINT index);
		UINT GetCurrentObjectIndex() const;
		void ResetObjectIndices();

		// Transient (frame) allocations
		void BeginFrame(UINT64 fenceCompleted);
		DescriptorRange AllocateCBV_SRV_UAV_Transient(UINT count);
		ID3D12DescriptorHeap* GetCurrentFrameSrvUavCbvHeap() const;
		void EndFrame(UINT64 fenceSignal);

		// Page getter for persistent range
		ID3D12DescriptorHeap* GetHeapForRange(const DescriptorRange& range) const;
		ID3D12DescriptorHeap* GetBindlessSrvHeap() const;

		UINT GetRtvHandleIncrementSize() const;
		UINT GetDsvHandleIncrementSize() const;
		UINT GetCbvSrvUavIncrementSize() const;

	private:
		ID3D12Device* m_Device;

		UINT m_NextObjCBIndex = 0;
		std::vector<UINT> m_FreeObjIndices;
		std::mutex m_ObjIndexMutex;

		// Persistent pools
		std::unique_ptr<DescriptorAllocator> m_RtvAllocator;
		std::unique_ptr<DescriptorAllocator> m_DsvAllocator;
		std::unique_ptr<DescriptorAllocator> m_CbvSrvUavPersistent;

		// Frame pool
		std::unique_ptr<FrameDescriptorHeap> m_FrameCbvSrvUav;
	};

}
