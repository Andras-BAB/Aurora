#include "aupch.h"

#include "Platform/DirectX/Renderer/DirectX12Context.h"

#include "Aurora/Core/Log.h"
#include "Aurora/Core/Timer.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "Aurora/Utils/PlatformUtils.h"

#include "Platform/DirectX/Utils/GeometryGenerator.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include <d3d12sdklayers.h>
#include <backends/imgui_impl_dx12.h>

namespace Aurora {

	DirectX12Context::DirectX12Context(WindowsWindow* windowHandle) : m_WindowHandle(windowHandle), m_SwapChain() {
	}

	void DirectX12Context::Init() {
		IDXGIFactory6* factory;
		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) {
			AU_CORE_ERROR("Failed to create DXGI Factory.");
			return;
		}

		IDXGIAdapter1* adapter;
		if (FAILED(factory->EnumAdapters1(0, &adapter))) {
			AU_CORE_ERROR("Failed to enumerate adapters.");
			return;
		}

		ID3D12Device* device;
		if (FAILED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)))) {
			AU_CORE_ERROR("Failed to create D3D12 Device.");
			return;
		}

		std::stringstream features;
		features << "Features: \n\t";

		D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
		if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, 
				sizeof(options5)))) {
			features << "Raytracing Tier: " << options5.RaytracingTier << "\n\t";
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
		if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, 
				sizeof(options6)))) {
			features << "Variable Shading Rate Tier: " << options6.VariableShadingRateTier << "\n\t";
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
		if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, 
				sizeof(options7)))) {

			features << "Mesh Shader Tier: " << options7.MeshShaderTier << "\n\t" <<
				"Sampler Feedback Tier: " << options7.SamplerFeedbackTier;
		}

		AU_CORE_INFO(features.str());

		device->Release();
		adapter->Release();
		factory->Release();

#ifdef AU_DEBUG

		// Enabling debug layer
		{
			MS::ComPtr<ID3D12Debug> debugController;
			ThrowOnFail(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}
		{
			MS::ComPtr<ID3D12Debug1> debugController1;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController1)))) {
				debugController1->SetEnableGPUBasedValidation(TRUE);
			}
		}

		
#endif
		HRESULT hr;
#ifdef AU_DEBUG
		hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_DxgiFactory));
		ThrowOnFail(hr);
#else
		//ThrowOnFail(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DxgiFactory)));
		hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DxgiFactory));
		if (FAILED(hr)) {
			AU_CORE_ERROR("CreateDXGIFactory2 failed: {}", hr);
			return;
		}

#endif

		// disabling Alt+Enter
		m_DxgiFactory->MakeWindowAssociation(
			glfwGetWin32Window(static_cast<GLFWwindow*>(m_WindowHandle->GetNativeWindow())), 
			DXGI_MWA_NO_ALT_ENTER);

		//HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
		hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));

		// Fallback to WARP device
		if(FAILED(hr)) {
			MS::ComPtr<IDXGIAdapter> pWarpAdapter;
			ThrowOnFail(m_DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
			ThrowOnFail(D3D12CreateDevice(
				pWarpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_Device)));
		} else {
			// AU_CORE_LOG_INFO("Physical device successfully created!");
		}

#ifdef AU_DEBUG
		{
			MS::ComPtr<ID3D12InfoQueue1> InfoQueue;
			if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&InfoQueue)))) {

				//InfoQueue->ClearRetrievalFilter();
				//InfoQueue->ClearStorageFilter();

				InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
				InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
				InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
				InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, false);

				auto messageCallback = [](D3D12_MESSAGE_CATEGORY Category,
					D3D12_MESSAGE_SEVERITY Severity,
					D3D12_MESSAGE_ID ID,
					LPCSTR pDescription,
					void* pContext) {
						switch (Severity) {
						case D3D12_MESSAGE_SEVERITY_CORRUPTION:
							AU_CORE_CRITICAL("[DX12] Corruption: {0}", pDescription);
							break;
						case D3D12_MESSAGE_SEVERITY_ERROR:
							AU_CORE_ERROR("[DX12] Error: {0}", pDescription);
							break;
						case D3D12_MESSAGE_SEVERITY_WARNING:
							AU_CORE_WARN("[DX12] Warning: {0}", pDescription);
							break;
						case D3D12_MESSAGE_SEVERITY_INFO:
						case D3D12_MESSAGE_SEVERITY_MESSAGE:
							AU_CORE_INFO("[DX12] Info: {0}", pDescription);
							break;
						}
					};

				DWORD callbackCookie = 0;
				HRESULT hrInfo = InfoQueue->RegisterMessageCallback(
					messageCallback,
					D3D12_MESSAGE_CALLBACK_FLAG_NONE,
					nullptr,
					&callbackCookie
				);

				if (FAILED(hrInfo)) {
					AU_CORE_ERROR("Failed to register D3D12 debug callback!");
				}
				//else {
				//	InfoQueue->AddApplicationMessage(D3D12_MESSAGE_SEVERITY_INFO, "Test message: The Callback works!");
				//	InfoQueue->AddApplicationMessage(D3D12_MESSAGE_SEVERITY_ERROR, "Test message: The Callback works!");
				//}
			}
		}
#endif

		ThrowOnFail(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = m_SwapChain.GetBackBufferFormat();
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		hr = m_Device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels));
		m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
		ThrowOnFail(hr);
		// AU_CORE_INFO("Multisample quality levels detected: {0}", m_4xMsaaQuality);
		assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef AU_DEBUG
		LogAdapters();
#endif

		m_FrameSyncs.resize(m_NumFrameResources);

		m_HeapManager = std::make_unique<DirectX12HeapManager>(m_Device.Get(), 512, 512, 500'000, 4096);
		
		CreateCommandObjects();

		CreateSwapChain();
		
		OnWindowResize();

		ThrowOnFail(m_CommandList->Reset(m_FrameSyncs[0]->CommandAllocator.Get(), nullptr));

		D3D12_RESOURCE_BARRIER barrier1 = {};
		barrier1.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier1.Transition.pResource = CurrentBackBuffer();
		barrier1.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier1.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_CommandList->ResourceBarrier(1, &barrier1);
		// Execute the init commands
		ThrowOnFail(m_CommandList->Close());
		ID3D12CommandList* cmdLists[] = {
			m_CommandList.Get()
		};
		m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		FlushCommandQueue();

		auto cmdListAlloc = m_FrameSyncs[0]->CommandAllocator;
		ThrowOnFail(cmdListAlloc->Reset());
		ThrowOnFail(m_CommandList->Reset(cmdListAlloc.Get(), nullptr));
		
		SubmitFrame();
	}

	void DirectX12Context::Shutdown() {
		if(m_Device != nullptr)
			FlushCommandQueue();
	}

	void DirectX12Context::SwapBuffers() {
		m_SwapChain.Present();
	}

	void DirectX12Context::OnWindowResize() {
		assert(m_Device);
		assert(m_FrameSyncs[m_CurrentFrameSyncIndex]->CommandAllocator);

		// Flush before changing any resources.
		FlushCommandQueue();

		ThrowOnFail(m_CommandList->Reset(m_FrameSyncs[m_CurrentFrameSyncIndex]->CommandAllocator.Get(), nullptr));
		
		m_SwapChain.Resize(m_WindowHandle->GetWidth(), m_WindowHandle->GetHeight(), m_Device.Get(), m_HeapManager.get(), m_CommandList.Get());

		// Execute the resize commands.
		ThrowOnFail(m_CommandList->Close());
		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until resize is complete.
		FlushCommandQueue();
	}

	ID3D12Device* DirectX12Context::GetDevice() const {
		return m_Device.Get();
	}

	ID3D12GraphicsCommandList* DirectX12Context::GetCommandList() const {
		return m_CommandList.Get();
	}

	void DirectX12Context::SubmitCommandList() const {
		ID3D12CommandList* cmdsLists[] = { GetCommandList() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}

	void DirectX12Context::WaitForFence(UINT64 fence) const {
		if (fence != 0 && m_Fence->GetCompletedValue() < fence) {
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			ThrowOnFail(m_Fence->SetEventOnCompletion(fence, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void DirectX12Context::SignalQueue() const {
		m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence);
	}

	uint32_t DirectX12Context::GetCurrentFrameSyncIndex() const {
		return m_CurrentFrameSyncIndex;
	}

	uint32_t DirectX12Context::GetFrameResourcesCount() const {
		return m_NumFrameResources;
	}

	ID3D12CommandQueue* DirectX12Context::GetCommandQueue() const {
		return m_CommandQueue.Get();
	}

	DirectX12HeapManager* DirectX12Context::GetHeapManager() const {
		return m_HeapManager.get();
	}

	void DirectX12Context::BeginFrame() {
		WaitForFence(m_FrameSyncs[m_CurrentFrameSyncIndex]->FenceValue);

		UINT64 completed = m_Fence->GetCompletedValue();
		m_HeapManager->BeginFrame(completed);

		auto cmdListAlloc = m_FrameSyncs[m_CurrentFrameSyncIndex]->CommandAllocator;
		ThrowOnFail(cmdListAlloc->Reset());

		ThrowOnFail(m_CommandList->Reset(cmdListAlloc.Get(), nullptr));
	}
	
	void DirectX12Context::SubmitFrame() {
		D3D12_RESOURCE_BARRIER b = {};
		b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		b.Transition.pResource = CurrentBackBuffer();
		b.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		b.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_CommandList->ResourceBarrier(1, &b);

		ThrowOnFail(m_CommandList->Close());

		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		m_CurrentFence++;
		m_FrameSyncs[m_CurrentFrameSyncIndex]->FenceValue = m_CurrentFence;

		m_HeapManager->EndFrame(m_FrameSyncs[m_CurrentFrameSyncIndex]->FenceValue);

		ThrowOnFail(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));

		m_CurrentFrameSyncIndex = (m_CurrentFrameSyncIndex + 1) % m_NumFrameResources;
	}
	
	void DirectX12Context::SetRenderTarget() {
		// --- Barrier ---
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = CurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_CommandList->ResourceBarrier(1, &barrier);

		// Specify the buffers we are going to render to.
		auto rtv = CurrentBackBufferView();
		auto dsv = DepthStencilView();
		m_CommandList->OMSetRenderTargets(1, &rtv, true, &dsv);
	}

	void DirectX12Context::CreateCommandObjects() {
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowOnFail(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

		for(uint32_t i = 0; i < m_NumFrameResources; i++) {
			m_FrameSyncs[i] = std::make_unique<FrameSync>(m_Device.Get());
		}

		ThrowOnFail(m_Device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_FrameSyncs[0]->CommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(m_CommandList.ReleaseAndGetAddressOf())
		));

		ThrowOnFail(m_CommandList->Close());
	}

	void DirectX12Context::CreateSwapChain() {
		m_SwapChain.Init(m_DxgiFactory.Get(), m_CommandQueue.Get(),
			glfwGetWin32Window(static_cast<GLFWwindow*>(m_WindowHandle->GetNativeWindow())),
			m_WindowHandle->GetWidth(), m_WindowHandle->GetHeight(), m_HeapManager.get());
	}

	void DirectX12Context::FlushCommandQueue() {
		m_CurrentFence++;

		ThrowOnFail(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));
		
		if (m_Fence->GetCompletedValue() < m_CurrentFence) {
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

			ThrowOnFail(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	ID3D12Resource* DirectX12Context::CurrentBackBuffer() const {
		return m_SwapChain.GetCurrentBackBuffer();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Context::CurrentBackBufferView() const {
		return m_SwapChain.GetCurrentBackBufferView();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Context::DepthStencilView() const {
		return m_SwapChain.GetDepthStencilView();
	}

	// Loggers for debug
	void DirectX12Context::LogAdapters() const {
		UINT i = 0;
		MS::ComPtr<IDXGIAdapter> adapter;

		// Use ComPtr to hold the adapter. When it goes out of scope or is reassigned, 
		// it automatically releases the previous reference.
		while (m_DxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			if (MS::wtos(desc.Description) == "Microsoft Basic Render Driver") {
				// No need to manually release; ComPtr handles it when loop continues/breaks
				break;
			}

			std::stringstream ss;
			ss << "*** Adapter: " << MS::wtos(desc.Description);
			AU_CORE_INFO(ss.str());

			LogAdapterOutputs(adapter.Get());

			// Prepare for next iteration (ComPtr releases the current one automatically on assignment)
			i++;
		}
	}

	void DirectX12Context::LogAdapterOutputs(IDXGIAdapter* adapter) const {
		if (!adapter)
			return;
		UINT i = 0;
		IDXGIOutput* output = nullptr;
		while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);

			std::stringstream ss;
			ss << "*** Output: " << MS::wtos(desc.DeviceName);
			AU_CORE_INFO(ss.str());

			LogOutputDisplayModes(output, m_SwapChain.GetBackBufferFormat());

			if (output) {
				output->Release();
				output = nullptr;
			}
			i++;
		}
	}

	void DirectX12Context::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format) {
		UINT count = 0;
		UINT flags = 0;

		output->GetDisplayModeList(format, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC> modeList(count);
		output->GetDisplayModeList(format, flags, &count, &modeList[0]);

		for (auto& x : modeList) {
			UINT n = x.RefreshRate.Numerator;
			UINT d = x.RefreshRate.Denominator;

			std::stringstream ss;
			ss << "Width = " << x.Width << " " << "Height = " << x.Height << " " << "Refresh = " << n << "/" << d;

			//AU_CORE_INFO(ss.str());
		}
	}
}
