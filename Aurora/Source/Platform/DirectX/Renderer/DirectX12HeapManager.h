#pragma once
#include "Platform/DirectX/Utils/MSUtils.h"

#include <mutex>

namespace Aurora {
    struct DescriptorHandleCPU { D3D12_CPU_DESCRIPTOR_HANDLE handle{}; };
    struct DescriptorHandleGPU { D3D12_GPU_DESCRIPTOR_HANDLE handle{}; };

    struct DescriptorRange {
        DescriptorHandleCPU cpuBase{};
        DescriptorHandleGPU gpuBase{}; // csak shader-visible heapnél van értelme
        UINT count = 0;
    };

    enum class HeapType : uint8_t {
	    RTV, 
    	DSV, 
    	CBV_SRV_UAV
    };

    struct DeferTicket { UINT64 fenceValue = 0; };

    // Egy descriptor heap page metaadataival és free-listjével
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

        DescriptorRange Allocate(UINT count);                  // persistent pool
        void Free(const DescriptorRange& range, DeferTicket ticket);  // deferred free
        void ReleaseCompleted(UINT64 fenceCompleted);          // free pending ranges

        ID3D12DescriptorHeap* GetPageForRange(const DescriptorRange& range) const;

    private:
        // page-ek és free-list kezelés
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
        DescriptorRange AllocateTransient(UINT count);         // lineáris allokáció
        ID3D12DescriptorHeap* GetCurrentHeap() const;
        void EndFrame(UINT64 fenceSignal);                     // egész heap deferred recycle

    private:
        // több heap page és write offset
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

        // Persistent allokációk
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

        // Transient (frame) allokációk
        void BeginFrame(UINT64 fenceCompleted);
        DescriptorRange AllocateCBV_SRV_UAV_Transient(UINT count);
        ID3D12DescriptorHeap* GetCurrentFrameSrvUavCbvHeap() const;
        void EndFrame(UINT64 fenceSignal);

        // Opcionális: page lekérdezés egy persistent range-hez
        ID3D12DescriptorHeap* GetHeapForRange(const DescriptorRange& range) const;

        UINT GetRtvHandleIncrementSize() const;
        UINT GetDsvHandleIncrementSize() const;
        UINT GetCbvSrvUavIncrementSize() const;

    private:
        ID3D12Device* m_Device;

        //std::atomic<UINT> m_NextObjCBIndex{ 0 };
        UINT m_NextObjCBIndex = 0;
        std::vector<UINT> m_FreeObjIndices;
        std::mutex m_ObjIndexMutex;

        // Persistent poolok
        std::unique_ptr<DescriptorAllocator> m_RtvAllocator;
        std::unique_ptr<DescriptorAllocator> m_DsvAllocator;
        std::unique_ptr<DescriptorAllocator> m_CbvSrvUavPersistent;

        // Frame pool
        std::unique_ptr<FrameDescriptorHeap> m_FrameCbvSrvUav;
    };

}
