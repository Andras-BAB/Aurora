#include "aupch.h"
#include "DirectX12HeapManager.h"

namespace Aurora {
	
	// -------- DescriptorPage --------

	DescriptorPage::DescriptorPage(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT capacity) 
	: m_Type(type), m_ShaderVisible(shaderVisible), m_Capacity(capacity) {
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = type;
		desc.NumDescriptors = capacity;
		desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
			: D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_Heap.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) {
			AU_CORE_LOG_ERROR("CreateDescriptorHeap failed");
		}

		m_DescriptorSize = device->GetDescriptorHandleIncrementSize(type);
		m_FreeRanges.emplace_back(0, capacity);
	}

	bool DescriptorPage::Allocate(UINT count, DescriptorRange& out) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		for (size_t i = 0; i < m_FreeRanges.size(); ++i) {
			auto [start, len] = m_FreeRanges[i];
			if (len >= count) {
				D3D12_CPU_DESCRIPTOR_HANDLE cpu = m_Heap->GetCPUDescriptorHandleForHeapStart();
				cpu.ptr += static_cast<SIZE_T>(start) * m_DescriptorSize;

				out.cpuBase.handle = cpu;
				out.count = count;

				if (m_ShaderVisible) {
					D3D12_GPU_DESCRIPTOR_HANDLE gpu = m_Heap->GetGPUDescriptorHandleForHeapStart();
					gpu.ptr += static_cast<UINT64>(start) * m_DescriptorSize;
					out.gpuBase.handle = gpu;
				}

				if (len == count) m_FreeRanges.erase(m_FreeRanges.begin() + i);
				else m_FreeRanges[i] = { start + count, len - count };

				return true;
			}
		}

		return false;
	}

	void DescriptorPage::FreeDeferred(const DescriptorRange& range, UINT64 fenceValue) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		SIZE_T basePtr = m_Heap->GetCPUDescriptorHandleForHeapStart().ptr;
		SIZE_T startPtr = range.cpuBase.handle.ptr - basePtr;
		UINT startIndex = static_cast<UINT>(startPtr / m_DescriptorSize);

		m_PendingFrees.push_back({ startIndex, range.count, fenceValue });
	}

	void DescriptorPage::ReleaseCompleted(UINT64 fenceCompleted) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto it = m_PendingFrees.begin();
		while (it != m_PendingFrees.end()) {
			if (it->fence <= fenceCompleted) {
				m_FreeRanges.emplace_back(it->start, it->len);
				it = m_PendingFrees.erase(it);
			} else {
				++it;
			}
		}

		if (m_FreeRanges.empty()) return;

		std::sort(m_FreeRanges.begin(), m_FreeRanges.end(),
			[](const auto& a, const auto& b) { return a.first < b.first; });

		std::vector<std::pair<UINT, UINT>> merged;
		merged.reserve(m_FreeRanges.size());
		for (auto [s, l] : m_FreeRanges) {
			if (!merged.empty()) {
				auto& last = merged.back();
				if (last.first + last.second == s) {
					last.second += l;
					continue;
				}
			}
			merged.emplace_back(s, l);
		}
		m_FreeRanges.swap(merged);
	}

	// -------- DescriptorAllocator --------

	DescriptorAllocator::DescriptorAllocator(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, UINT pageSize) 
	: m_Device(device), m_Type(type), m_ShaderVisible(shaderVisible), m_PageSize(pageSize) {
		if (!m_Device) {
			AU_CORE_LOG_ERROR("DescriptorAllocator: device is null");
		}
		if (m_PageSize == 0) {
			AU_CORE_LOG_ERROR("DescriptorAllocator: pageSize must be > 0");
		}
	}

	DescriptorRange DescriptorAllocator::Allocate(UINT count) {
#ifdef max
#undef max
#endif
		// Próbáljuk a meglévő page-eket
		for (auto& p : m_Pages) {
			DescriptorRange r{};
			if (p->Allocate(count, r)) return r;
		}
		// Új page létrehozása (legalább count méretben)
		auto page = std::make_unique<DescriptorPage>(m_Device, m_Type, m_ShaderVisible, std::max(m_PageSize, count));
		DescriptorRange r{};
		bool ok = page->Allocate(count, r);
		m_Pages.emplace_back(std::move(page));
		if (!ok) {
			AU_CORE_LOG_ERROR("DescriptorAllocator: allocation failed unexpectedly");
		}
		return r;
	}

	void DescriptorAllocator::Free(const DescriptorRange& range, DeferTicket ticket) {
		for (auto& p : m_Pages) {
			SIZE_T base = p->GetCpuStart().ptr;
			SIZE_T end = base + static_cast<SIZE_T>(p->GetCapacity()) * p->GetDescriptorSize();
			SIZE_T ptr = range.cpuBase.handle.ptr;
			if (ptr >= base && ptr < end) {
				p->FreeDeferred(range, ticket.fenceValue);
				return;
			}
		}
		// Ha nem tartozik ide, csendben ignoráljuk (vagy dobjunk hibát). Itt inkább megtűrjük.
	}

	void DescriptorAllocator::ReleaseCompleted(UINT64 fenceCompleted) {
		for (auto& p : m_Pages) {
			p->ReleaseCompleted(fenceCompleted);
		}
	}

	ID3D12DescriptorHeap* DescriptorAllocator::GetPageForRange(const DescriptorRange& range) const {
		for (auto& p : m_Pages) {
			SIZE_T base = p->GetCpuStart().ptr;
			SIZE_T end = base + static_cast<SIZE_T>(p->GetCapacity()) * p->GetDescriptorSize();
			SIZE_T ptr = range.cpuBase.handle.ptr;
			if (ptr >= base && ptr < end) {
				return p->GetHeap();
			}
		}
		return nullptr;
	}

	// -------- FrameDescriptorHeap --------

	FrameDescriptorHeap::FrameDescriptorHeap(ID3D12Device* device, UINT pageSize) : m_Device(device), m_PageSize(pageSize) {
		if (!m_Device) {
			AU_CORE_LOG_ERROR("FrameDescriptorHeap: device is null");
		}
		if (m_PageSize == 0) {
			AU_CORE_LOG_ERROR("FrameDescriptorHeap: pageSize must be > 0");
		}
		AllocateNewPage();
	}

	void FrameDescriptorHeap::BeginFrame(UINT64 fenceCompleted) {
		for (auto& p : m_Pages) {
			if (p.fenceSignal && p.fenceSignal <= fenceCompleted) {
				p.offset = 0;
				p.fenceSignal = 0;
			}
		}
		SelectActivePage();
	}

	DescriptorRange FrameDescriptorHeap::AllocateTransient(UINT count) {
		auto& p = m_Pages[m_Active];
		if (p.offset + count > p.capacity) {
			AllocateNewPage();
		}
		auto& cur = m_Pages[m_Active];

		DescriptorRange r{};
		UINT start = cur.offset;
		cur.offset += count;

		D3D12_CPU_DESCRIPTOR_HANDLE cpu = cur.heap->GetCPUDescriptorHandleForHeapStart();
		cpu.ptr += static_cast<SIZE_T>(start) * cur.descriptorSize;
		r.cpuBase.handle = cpu;
		r.count = count;

		D3D12_GPU_DESCRIPTOR_HANDLE gpu = cur.heap->GetGPUDescriptorHandleForHeapStart();
		gpu.ptr += static_cast<UINT64>(start) * cur.descriptorSize;
		r.gpuBase.handle = gpu;

		return r;
	}

	ID3D12DescriptorHeap* FrameDescriptorHeap::GetCurrentHeap() const {
		return m_Pages[m_Active].heap.Get();
	}

	void FrameDescriptorHeap::EndFrame(UINT64 fenceSignal) {
		// jelöld az aktív page-et foglaltnak a fence-ig
		m_Pages[m_Active].fenceSignal = fenceSignal;
	}

	void FrameDescriptorHeap::AllocateNewPage() {
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = m_PageSize;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		FramePage p{};
		HRESULT hr = m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(p.heap.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) throw std::runtime_error("CreateDescriptorHeap (frame) failed");

		p.descriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		p.capacity = m_PageSize;
		p.offset = 0;
		p.fenceSignal = 0;

		m_Pages.push_back(std::move(p));
		m_Active = static_cast<UINT>(m_Pages.size() - 1);
	}

	void FrameDescriptorHeap::SelectActivePage() {
		for (UINT i = 0; i < m_Pages.size(); ++i) {
			if (m_Pages[i].fenceSignal == 0 && m_Pages[i].offset < m_Pages[i].capacity) {
				m_Active = i;
				return;
			}
		}
		AllocateNewPage();
	}

	// -------- DirectX12HeapManager --------

	DirectX12HeapManager::DirectX12HeapManager(ID3D12Device* device, UINT rtvPageSize, UINT dsvPageSize,
		UINT cbvSrvUavPersistentPageSize, UINT cbvSrvUavFramePageSize) : m_Device(device){

		if (!m_Device) {
			AU_CORE_LOG_ERROR("DirectX12HeapManager: device is null");
		}

		m_RtvAllocator = std::make_unique<DescriptorAllocator>(
			m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, rtvPageSize);

		m_DsvAllocator = std::make_unique<DescriptorAllocator>(
			m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, dsvPageSize);

		m_CbvSrvUavPersistent = std::make_unique<DescriptorAllocator>(
			m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, false, cbvSrvUavPersistentPageSize);

		m_FrameCbvSrvUav = std::make_unique<FrameDescriptorHeap>(
			m_Device, cbvSrvUavFramePageSize);
	}

	DescriptorRange DirectX12HeapManager::AllocateRTV(UINT count) {
		return m_RtvAllocator->Allocate(count);
	}

	void DirectX12HeapManager::FreeRTV(const DescriptorRange& range, DeferTicket ticket) {
		m_RtvAllocator->Free(range, ticket);
	}

	DescriptorRange DirectX12HeapManager::AllocateDSV(UINT count) {
		return m_DsvAllocator->Allocate(count);
	}

	void DirectX12HeapManager::FreeDSV(const DescriptorRange& range, DeferTicket ticket) {
		m_DsvAllocator->Free(range, ticket);
	}

	DescriptorRange DirectX12HeapManager::AllocateCBV_SRV_UAV_Persistent(UINT count) {
		return m_CbvSrvUavPersistent->Allocate(count);
	}

	void DirectX12HeapManager::FreeCBV_SRV_UAV_Persistent(const DescriptorRange& range, DeferTicket ticket) {
		m_CbvSrvUavPersistent->Free(range, ticket);
	}

	UINT DirectX12HeapManager::AllocateObjCBIndex() {
		//return m_NextObjCBIndex++;
		std::lock_guard<std::mutex> lock(m_ObjIndexMutex);
		if (!m_FreeObjIndices.empty()) {
			UINT idx = m_FreeObjIndices.back();
			m_FreeObjIndices.pop_back();
			return idx;
		}
		return m_NextObjCBIndex++;
	}

	void DirectX12HeapManager::FreeObjCBIndex(UINT index) {
		std::lock_guard<std::mutex> lock(m_ObjIndexMutex);
		m_FreeObjIndices.push_back(index);
	}

	UINT DirectX12HeapManager::GetCurrentObjectIndex() const {
		return m_NextObjCBIndex;
	}

	void DirectX12HeapManager::ResetObjectIndices() {
		//m_NextObjCBIndex = 0;
		std::lock_guard<std::mutex> lock(m_ObjIndexMutex);
		m_NextObjCBIndex = 0;
		m_FreeObjIndices.clear();
	}

	void DirectX12HeapManager::BeginFrame(UINT64 fenceCompleted) {
		// Persistent deferred free-k kiürítése
		m_RtvAllocator->ReleaseCompleted(fenceCompleted);
		m_DsvAllocator->ReleaseCompleted(fenceCompleted);
		m_CbvSrvUavPersistent->ReleaseCompleted(fenceCompleted);

		// Transient heap reciklálás
		m_FrameCbvSrvUav->BeginFrame(fenceCompleted);
	}

	DescriptorRange DirectX12HeapManager::AllocateCBV_SRV_UAV_Transient(UINT count) {
		return m_FrameCbvSrvUav->AllocateTransient(count);
	}

	ID3D12DescriptorHeap* DirectX12HeapManager::GetCurrentFrameSrvUavCbvHeap() const {
		return m_FrameCbvSrvUav->GetCurrentHeap();
	}

	void DirectX12HeapManager::EndFrame(UINT64 fenceSignal) {
		m_FrameCbvSrvUav->EndFrame(fenceSignal);
	}

	ID3D12DescriptorHeap* DirectX12HeapManager::GetHeapForRange(const DescriptorRange& range) const {
		if (auto h = m_RtvAllocator->GetPageForRange(range)) return h;
		if (auto h = m_DsvAllocator->GetPageForRange(range)) return h;
		if (auto h = m_CbvSrvUavPersistent->GetPageForRange(range)) return h;
		return nullptr;
	}

	UINT DirectX12HeapManager::GetRtvHandleIncrementSize() const {
		return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	UINT DirectX12HeapManager::GetDsvHandleIncrementSize() const {
		return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	UINT DirectX12HeapManager::GetCbvSrvUavIncrementSize() const {
		return m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}
