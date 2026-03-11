#include "aupch.h"
#include "Aurora/Core/Log.h"
#include "Platform/DirectX/Renderer/DirectX12Context.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <DirectXColors.h>
#include <d3d12sdklayers.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_glfw.h>

#include "imgui.h"
#include "Aurora/Core/Timer.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "Aurora/Utils/PlatformUtils.h"
#include "Platform/DirectX/Utils/GeometryGenerator.h"

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

		AU_CORE_LOG_INFO(features.str());

		device->Release();
		adapter->Release();
		factory->Release();

#ifdef DEBUG

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
#ifdef DEBUG
		hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_DxgiFactory));
		ThrowOnFail(hr);
#else
		//ThrowOnFail(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DxgiFactory)));
		hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DxgiFactory));
		if (FAILED(hr)) {
			AU_CORE_LOG_ERROR("CreateDXGIFactory2 failed: {}", hr);
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

#ifdef DEBUG
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
							AU_CORE_LOG_CRITICAL("[DX12] Corruption: {0}", pDescription);
							break;
						case D3D12_MESSAGE_SEVERITY_ERROR:
							AU_CORE_LOG_ERROR("[DX12] Error: {0}", pDescription);
							break;
						case D3D12_MESSAGE_SEVERITY_WARNING:
							AU_CORE_LOG_WARN("[DX12] Warning: {0}", pDescription);
							break;
						case D3D12_MESSAGE_SEVERITY_INFO:
						case D3D12_MESSAGE_SEVERITY_MESSAGE:
							AU_CORE_LOG_INFO("[DX12] Info: {0}", pDescription);
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
					AU_CORE_LOG_ERROR("Failed to register D3D12 debug callback!");
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
		// AU_CORE_LOG_INFO("Multisample quality levels detected: {0}", m_4xMsaaQuality);
		assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef DEBUG
		LogAdapters();
#endif
		//m_FrameResources.resize(m_NumFrameResources);
		m_FrameSyncs.resize(m_NumFrameResources);
		//m_FrameRenderData.resize(m_NumFrameResources);

		m_HeapManager = std::make_unique<DirectX12HeapManager>(m_Device.Get(), 512, 512, 2048, 4096);
		
		CreateCommandObjects();

		CreateSwapChain();
		
		//CreateRtvAndDsvHeaps();

		OnWindowResize();

		//ThrowOnFail(m_CommandList->Reset(m_FrameResources[0]->CmdListAlloc.Get(), nullptr));
		ThrowOnFail(m_CommandList->Reset(m_FrameSyncs[0]->CommandAllocator.Get(), nullptr));
		
		PipelineStateProperties props = {};
		props.backBufferFormat = m_SwapChain.GetBackBufferFormat();
		props.depthStencilFormat = m_SwapChain.GetDepthStencilFormat();
		props.device = m_Device.Get();
		props.inputLayout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		props.vertexShader = std::make_shared<DirectX12VertexShader>("shaders/color.hlsl", "baseVert");
		props.pixelShader = std::make_shared<DirectX12PixelShader>("shaders/color.hlsl", "basePixel");
		props.isWireframe = false;
		//m_PipelineLib.CreatePipeline(props, "basePipeline");
		
		//BuildShapeGeometry(); // Needs opened cmdList
		//BuildMaterials();
		//BuildRenderItems();
		//BuildFrameResources();
		BuildDescriptorHeaps();
		//BuildConstantBufferViews();

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

		//m_Geometries["shapeGeo"]->DisposeUploaders();

		auto cmdListAlloc = m_FrameSyncs[0]->CommandAllocator;
		ThrowOnFail(cmdListAlloc->Reset());
		ThrowOnFail(m_CommandList->Reset(cmdListAlloc.Get(), nullptr));
		
		SubmitFrame();
	}

	void DirectX12Context::Shutdown() {
		if(m_Device != nullptr)
			FlushCommandQueue();
	}

	void DirectX12Context::SwapBuffers(Timestep ts) {
		//if (!isFirstFrame) {
		//	UpdateCamera(ts);
		//} else {
		//	isFirstFrame = false;
		//}

		m_SwapChain.Present();
	}

	void DirectX12Context::OnWindowResize() {
		assert(m_Device);
		//assert(m_FrameResources[m_CurrFrameResourceIndex]->CmdListAlloc);
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

		// The window resized, so update the aspect ratio and recompute the projection matrix.
		DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, 
			static_cast<float>(m_WindowHandle->GetWidth()) / static_cast<float>(m_WindowHandle->GetHeight()), 
			1.0f, 1000.0f);

		// TODO: update camera proj
		//XMStoreFloat4x4(&m_Proj, P);
		//camera.GetCamera().SetLens(0.25f * MathHelper::Pi, 
		//	static_cast<float>(m_WindowHandle->GetWidth()) / static_cast<float>(m_WindowHandle->GetHeight()), 1.f, 1000.f);
	}

	ID3D12Device* DirectX12Context::GetDevice() const {
		return m_Device.Get();
	}

	ID3D12GraphicsCommandList* DirectX12Context::GetCommandList() const {
		return m_CommandList.Get();
	}

	void DirectX12Context::SubmitCommandList() const {
		// Add the command list to the queue for execution.
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
		//WaitForFence(m_CurrFrameResource->Fence);
		WaitForFence(m_FrameSyncs[m_CurrentFrameSyncIndex]->FenceValue);

		UINT64 completed = m_Fence->GetCompletedValue();
		m_HeapManager->BeginFrame(completed);

		//auto cmdListAlloc = m_CurrFrameResource->CmdListAlloc;
		auto cmdListAlloc = m_FrameSyncs[m_CurrentFrameSyncIndex]->CommandAllocator;
		ThrowOnFail(cmdListAlloc->Reset());

		//ThrowOnFail(m_CommandList->Reset(cmdListAlloc.Get(), m_PipelineLib.Get("basePipeline")->GetPipelineState()));		
		ThrowOnFail(m_CommandList->Reset(cmdListAlloc.Get(), nullptr));		
		//m_CurrentFrameRenderData = m_FrameRenderData[m_CurrentFrameSyncIndex].get();
	
		D3D12_RESOURCE_BARRIER barrier1 = {};
		barrier1.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier1.Transition.pResource = CurrentBackBuffer();
		barrier1.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier1.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_CommandList->ResourceBarrier(1, &barrier1);


		// Specify the buffers we are going to render to.
		auto tempbbv = CurrentBackBufferView();
		auto tempdsv = DepthStencilView();
		m_CommandList->OMSetRenderTargets(1, &tempbbv, true, &tempdsv);

		//ID3D12DescriptorHeap* descriptorHeaps[] = { m_HeapManager->GetCurrentFrameSrvUavCbvHeap() };
		//m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		//m_CommandList->SetGraphicsRootSignature(m_PipelineLib.Get("basePipeline")->GetRootSignature());

		
		//UpdateObjectCBs();
		//UpdateMaterialCBs();
		//UpdateMainPassCB(ts);
		//UpdateMainPassCB(0);
	}
	
	void DirectX12Context::SubmitFrame() {
		D3D12_RESOURCE_BARRIER b = {};
		b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		b.Transition.pResource = CurrentBackBuffer();
		b.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		b.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_CommandList->ResourceBarrier(1, &b);
		//------------

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
		
		// --- RTV + DSV beállítása ---
		// Specify the buffers we are going to render to.
		auto rtv = CurrentBackBufferView();
		auto dsv = DepthStencilView();
		m_CommandList->OMSetRenderTargets(1, &rtv, true, &dsv);

		// --- CBV/SRV/UAV heap beállítása (frame heap a HeapManagerből) ---
		// TODO: move to somewhere else
		//ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvSrvUavHeap.Get() };
		//ID3D12DescriptorHeap* descriptorHeaps[] = { m_HeapManager->GetCurrentFrameSrvUavCbvHeap() };
		//m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// --- Root signature ---
		//m_CommandList->SetGraphicsRootSignature(m_PipelineLib.Get("basePipeline")->GetRootSignature());

		// --- Pass CBV GPU handle a HeapManager frame-allokációból ---
		// Ezt valahol frame elején kell allokálnod:
		// m_PassCbvRange = m_HeapManager->AllocateCBV_SRV_UAV_Transient(frameCount);

		//D3D12_GPU_DESCRIPTOR_HANDLE passCbvHandle = m_PassCbvRange.gpuBase.handle;

		// Minden frame-hez külön CBV (ha úgy használod)
		//passCbvHandle.ptr += m_CurrFrameResourceIndex * m_HeapManager->GetCbvSrvUavIncrementSize();
		//passCbvHandle.ptr += m_CurrentFrameSyncIndex * m_HeapManager->GetCbvSrvUavIncrementSize();

		//m_CommandList->SetGraphicsRootDescriptorTable(
		//	1, passCbvHandle
		//);
	}

	void DirectX12Context::CreateCommandObjects() {
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowOnFail(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

		for(uint32_t i = 0; i < m_NumFrameResources; i++) {
			//m_FrameResources[i] = std::make_unique<FrameResource>(m_Device.Get());
			m_FrameSyncs[i] = std::make_unique<FrameSync>(m_Device.Get());
			//m_FrameRenderData[i] = std::make_unique<FrameRenderData>();

			//ThrowOnFail(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			//	m_FrameResources[i]->CmdListAlloc.Get(),
			//	nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));
			//ThrowOnFail(m_CommandList->Close());
		}

		ThrowOnFail(m_Device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			//m_FrameResources[0]->CmdListAlloc.Get(),
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

	void DirectX12Context::BuildDescriptorHeaps() {
		//UINT objCount = (UINT)m_OpaqueRitems.size();

		//// Need a CBV descriptor for each object for each frame resource,
		//// +1 for the perPass CBV for each frame resource.
		//UINT numDescriptors = (objCount + 1) * m_NumFrameResources;

		//// Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
		//m_PassCbvOffset = objCount * m_NumFrameResources;

		//D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
		//cbvHeapDesc.NumDescriptors = numDescriptors;
		//cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		//cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		//cbvHeapDesc.NodeMask = 0;
		//ThrowOnFail(m_Device->CreateDescriptorHeap(&cbvHeapDesc,
		//	IID_PPV_ARGS(&m_CbvSrvUavHeap)));

		//m_PassCbvRange = m_HeapManager->AllocateCBV_SRV_UAV_Persistent(m_NumFrameResources);
		//m_MaterialCbvRange = m_HeapManager->AllocateCBV_SRV_UAV_Persistent(m_NumFrameResources * static_cast<UINT>(m_Materials.size()));
		//m_ObjectCbvRange = m_HeapManager->AllocateCBV_SRV_UAV_Persistent(m_NumFrameResources * objCount);
	}
	/*
	void DirectX12Context::BuildShapeGeometry() {
		d3dUtil::GeometryGenerator geoGen;
		d3dUtil::GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
		d3dUtil::GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
		d3dUtil::GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
		d3dUtil::GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

		//
		// We are concatenating all the geometry into one big vertex/index buffer.  So
		// define the regions in the buffer each submesh covers.
		//

		// Cache the vertex offsets to each object in the concatenated vertex buffer.
		UINT boxVertexOffset = 0;
		UINT gridVertexOffset = (UINT)box.Vertices.size();
		UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
		UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

		// Cache the starting index for each object in the concatenated index buffer.
		UINT boxIndexOffset = 0;
		UINT gridIndexOffset = (UINT)box.Indices32.size();
		UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
		UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

		// Define the SubmeshGeometry that cover different 
		// regions of the vertex/index buffers.

		d3dUtil::SubmeshGeometry boxSubmesh;
		boxSubmesh.IndexCount = (UINT)box.Indices32.size();
		boxSubmesh.StartIndexLocation = boxIndexOffset;
		boxSubmesh.BaseVertexLocation = boxVertexOffset;

		d3dUtil::SubmeshGeometry gridSubmesh;
		gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
		gridSubmesh.StartIndexLocation = gridIndexOffset;
		gridSubmesh.BaseVertexLocation = gridVertexOffset;

		d3dUtil::SubmeshGeometry sphereSubmesh;
		sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
		sphereSubmesh.StartIndexLocation = sphereIndexOffset;
		sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

		d3dUtil::SubmeshGeometry cylinderSubmesh;
		cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
		cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
		cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

		//
		// Extract the vertex elements we are interested in and pack the
		// vertices of all the meshes into one vertex buffer.
		//

		auto totalVertexCount =
			box.Vertices.size() +
			grid.Vertices.size() +
			sphere.Vertices.size() +
			cylinder.Vertices.size();

		std::vector<Vertex> vertices(totalVertexCount);

		UINT k = 0;
		for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = box.Vertices[i].Position;
			vertices[k].Normal = box.Vertices[i].Normal;
		}

		for(size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = grid.Vertices[i].Position;
			vertices[k].Normal = grid.Vertices[i].Normal;
		}

		for(size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = sphere.Vertices[i].Position;
			vertices[k].Normal = sphere.Vertices[i].Normal;
		}

		for(size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = cylinder.Vertices[i].Position;
			vertices[k].Normal = cylinder.Vertices[i].Normal;
		}

		std::vector<std::uint16_t> indices;
		indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
		indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
		indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
		indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size()  * sizeof(std::uint16_t);

		auto geo = std::make_unique<d3dUtil::MeshGeometry>();
		geo->Name = "shapeGeo";

		ThrowOnFail(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowOnFail(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dUtil::utils::CreateDefaultBuffer(m_Device.Get(),
			m_CommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

		geo->IndexBufferGPU = d3dUtil::utils::CreateDefaultBuffer(m_Device.Get(),
			m_CommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(Vertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		geo->DrawArgs["box"] = boxSubmesh;
		geo->DrawArgs["grid"] = gridSubmesh;
		geo->DrawArgs["sphere"] = sphereSubmesh;
		geo->DrawArgs["cylinder"] = cylinderSubmesh;

		m_Geometries[geo->Name] = std::move(geo);
	}
	
	void DirectX12Context::BuildMaterials() {
		auto red = std::make_unique<d3dUtil::Material>();
		red->Name = "red";
		red->MatCBIndex = 0;
		red->DiffuseAlbedo = DirectX::XMFLOAT4(DirectX::Colors::Red);
		red->FresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
		red->Roughness = 0.125f;
		red->NumFramesDirty = m_NumFrameResources;

		auto green = std::make_unique<d3dUtil::Material>();
		green->Name = "green";
		green->MatCBIndex = 1;
		green->DiffuseAlbedo = DirectX::XMFLOAT4(DirectX::Colors::ForestGreen);
		green->FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
		green->Roughness = 0.0f;
		green->NumFramesDirty = m_NumFrameResources;

		auto darkGreen = std::make_unique<d3dUtil::Material>();
		darkGreen->Name = "darkGreen";
		darkGreen->MatCBIndex = 2;
		darkGreen->DiffuseAlbedo = DirectX::XMFLOAT4(DirectX::Colors::DarkGreen);
		darkGreen->FresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
		darkGreen->Roughness = 0.125f;
		darkGreen->NumFramesDirty = m_NumFrameResources;

		auto blue = std::make_unique<d3dUtil::Material>();
		blue->Name = "blue";
		blue->MatCBIndex = 3;
		blue->DiffuseAlbedo = DirectX::XMFLOAT4(DirectX::Colors::DeepSkyBlue);
		blue->FresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
		blue->Roughness = 0.125f;
		blue->NumFramesDirty = m_NumFrameResources;

		m_Materials["red"] = std::move(red);
		m_Materials["green"] = std::move(green);
		m_Materials["darkGreen"] = std::move(darkGreen);
		m_Materials["blue"] = std::move(blue);
	}
	*/
	/*
	void DirectX12Context::UpdateMaterialCBs() {
		//auto currMaterialCB = m_CurrentFrameRenderData->MaterialCB.get();
		for (auto& e : m_Materials) {
			// Only update the cbuffer data if the constants have changed. If
			// the cbuffer data changes, it needs to be updated for each 
			// FrameResource.
			d3dUtil::Material* mat = e.second.get();
			if (mat->NumFramesDirty > 0) {
				DirectX::XMMATRIX matTransform = DirectX::XMLoadFloat4x4(&mat->MatTransform);
				MaterialConstants matConstants;
				matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
				matConstants.FresnelR0 = mat->FresnelR0;
				matConstants.Roughness = mat->Roughness;
				//currMaterialCB->CopyData(mat->MatCBIndex, matConstants);
				// Next FrameResource need to be updated too.
				mat->NumFramesDirty--;
			}
		}
	}
	*/
	/*
	void DirectX12Context::BuildRenderItems() {
		auto boxRitem = std::make_unique<RenderItem>();
		XMStoreFloat4x4(&boxRitem->World, DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f) * DirectX::XMMatrixTranslation(0.0f, 0.5f, 0.0f));
		boxRitem->ObjCBIndex = 0;
		boxRitem->Mat = m_Materials["darkGreen"].get();
		boxRitem->Geo = m_Geometries["shapeGeo"].get();
		boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
		boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
		boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
		boxRitem->NumFramesDirty = m_NumFrameResources;
		m_AllRitems.push_back(std::move(boxRitem));

		auto gridRitem = std::make_unique<RenderItem>();
		gridRitem->World = MathHelper::Identity4x4();
		gridRitem->ObjCBIndex = 1;
		gridRitem->Mat = m_Materials["green"].get();
		gridRitem->Geo = m_Geometries["shapeGeo"].get();
		gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
		gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
		gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
		gridRitem->NumFramesDirty = m_NumFrameResources;
		m_AllRitems.push_back(std::move(gridRitem));

		UINT objCBIndex = 2;
		for(int i = 0; i < 5; ++i)
		{
			auto leftCylRitem = std::make_unique<RenderItem>();
			auto rightCylRitem = std::make_unique<RenderItem>();
			auto leftSphereRitem = std::make_unique<RenderItem>();
			auto rightSphereRitem = std::make_unique<RenderItem>();

			DirectX::XMMATRIX leftCylWorld = DirectX::XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
			DirectX::XMMATRIX rightCylWorld = DirectX::XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);

			DirectX::XMMATRIX leftSphereWorld = DirectX::XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
			DirectX::XMMATRIX rightSphereWorld = DirectX::XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);

			XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
			leftCylRitem->ObjCBIndex = objCBIndex++;
			leftCylRitem->Mat = m_Materials["blue"].get();
			leftCylRitem->Geo = m_Geometries["shapeGeo"].get();
			leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
			leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
			leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
			leftCylRitem->NumFramesDirty = m_NumFrameResources;

			XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
			rightCylRitem->ObjCBIndex = objCBIndex++;
			rightCylRitem->Mat = m_Materials["blue"].get();
			rightCylRitem->Geo = m_Geometries["shapeGeo"].get();
			rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
			rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
			rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
			rightCylRitem->NumFramesDirty = m_NumFrameResources;

			XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
			leftSphereRitem->ObjCBIndex = objCBIndex++;
			leftSphereRitem->Mat = m_Materials["red"].get();
			leftSphereRitem->Geo = m_Geometries["shapeGeo"].get();
			leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
			leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
			leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
			leftSphereRitem->NumFramesDirty = m_NumFrameResources;

			XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
			rightSphereRitem->ObjCBIndex = objCBIndex++;
			rightSphereRitem->Mat = m_Materials["red"].get();
			rightSphereRitem->Geo = m_Geometries["shapeGeo"].get();
			rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
			rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
			rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
			rightSphereRitem->NumFramesDirty = m_NumFrameResources;

			m_AllRitems.push_back(std::move(leftCylRitem));
			m_AllRitems.push_back(std::move(rightCylRitem));
			m_AllRitems.push_back(std::move(leftSphereRitem));
			m_AllRitems.push_back(std::move(rightSphereRitem));
		}

		// All the render items are opaque.
		for(auto& e : m_AllRitems) {
			m_OpaqueRitems.push_back(e.get());
		}

		std::sort(m_OpaqueRitems.begin(), m_OpaqueRitems.end(),
			[](const RenderItem* a, const RenderItem* b) {
				// Cím szerinti összehasonlítás elég, ha egyedi material instance-ok vannak
				return a->Mat < b->Mat;
			});
	}
	*/
	/*
	void DirectX12Context::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems) {
		UINT descriptorSize = m_HeapManager->GetCbvSrvUavIncrementSize();

		UINT objCount = (UINT)m_OpaqueRitems.size();
		UINT matCount = (UINT)m_Materials.size();

		// copying from persistent to transient heap -------------
		// pass constants
		DescriptorRange m_FramePassCbvRange = m_HeapManager->AllocateCBV_SRV_UAV_Transient(1);

		// persistent pass CBV handle (aktuális frame-hez tartozó)
		D3D12_CPU_DESCRIPTOR_HANDLE srcPass = m_PassCbvRange.cpuBase.handle;
		//srcPass.ptr += m_CurrFrameResourceIndex * descriptorSize;
		srcPass.ptr += m_CurrentFrameSyncIndex * descriptorSize;

		// transient cél
		D3D12_CPU_DESCRIPTOR_HANDLE dstPass = m_FramePassCbvRange.cpuBase.handle;

		m_Device->CopyDescriptorsSimple(
			1,
			dstPass,
			srcPass,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

		d3dUtil::Material* lastMaterial = nullptr;

		// object CBVs
		DescriptorRange m_FrameObjectCbvRange = m_HeapManager->AllocateCBV_SRV_UAV_Transient(objCount);

		// persistent object CBV-k kezdete az aktuális frame-hez
		D3D12_CPU_DESCRIPTOR_HANDLE srcObj = m_ObjectCbvRange.cpuBase.handle;
		//srcObj.ptr += (SIZE_T)(m_CurrFrameResourceIndex * objCount) * descriptorSize;
		srcObj.ptr += static_cast<SIZE_T>(m_CurrentFrameSyncIndex * objCount) * descriptorSize;

		D3D12_CPU_DESCRIPTOR_HANDLE dstObj = m_FrameObjectCbvRange.cpuBase.handle;

		m_Device->CopyDescriptorsSimple(
			objCount,
			dstObj,
			srcObj,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

		// material CBVs
		DescriptorRange m_FrameMaterialCbvRange = m_HeapManager->AllocateCBV_SRV_UAV_Transient(matCount);

		D3D12_CPU_DESCRIPTOR_HANDLE srcMat = m_MaterialCbvRange.cpuBase.handle;
		//srcMat.ptr += (SIZE_T)(m_CurrFrameResourceIndex * matCount) * descriptorSize;
		srcMat.ptr += static_cast<SIZE_T>(m_CurrentFrameSyncIndex * matCount) * descriptorSize;

		D3D12_CPU_DESCRIPTOR_HANDLE dstMat = m_FrameMaterialCbvRange.cpuBase.handle;
		m_Device->CopyDescriptorsSimple(
			matCount,
			dstMat,
			srcMat,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

		cmdList->SetGraphicsRootDescriptorTable(2, m_FramePassCbvRange.gpuBase.handle);

		// For each render item...
		for(size_t i = 0; i < ritems.size(); ++i) {
			auto ri = ritems[i];

			// --- 1. ANYAG BEÁLLÍTÁSA (Csak ha változott!) ---
			if (ri->Mat != lastMaterial) {
				lastMaterial = ri->Mat;
				// A) Material Constant Buffer beállítása
				// (Ez feltételezi, hogy a CBV-k sorban vannak, ahogy nálad is)
				D3D12_GPU_DESCRIPTOR_HANDLE matHandle = m_FrameMaterialCbvRange.gpuBase.handle;
				matHandle.ptr += (SIZE_T)lastMaterial->MatCBIndex * descriptorSize;
				cmdList->SetGraphicsRootDescriptorTable(1, matHandle);

				// B) TEXTŰRáK MÁSOLÁSA ÉS BEKÖTÉSE (Ha van textúra)
				// Itt jön a trükk: Csak akkor másolunk descriptor-t a Transient heapre,
				// amikor új anyagra váltunk. Nem objektumonként!

				// Ha lenne textúrád, itt másolnád át a Persistent -> Transient heapre,
				// és kötnöd be a (pl.) 3-as root paraméterre.
			}

			// --- 2. OBJEKTUM SPECIFIKUS ADATOK ---
			// Ez sajnos objektumonként változik (World Mátrix), ezt nem ússzuk meg
			D3D12_GPU_DESCRIPTOR_HANDLE objHandle = m_FrameObjectCbvRange.gpuBase.handle;
			objHandle.ptr += (SIZE_T)ri->ObjCBIndex * descriptorSize;
			cmdList->SetGraphicsRootDescriptorTable(0, objHandle);

			// --- VB/IB/Topology beállítása ---
			auto tempVB = ri->Geo->VertexBufferView();
			auto tempIB = ri->Geo->IndexBufferView();
			cmdList->IASetVertexBuffers(0, 1, &tempVB);
			cmdList->IASetIndexBuffer(&tempIB);
			cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

			cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
		}
	}
	*/
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
			AU_CORE_LOG_INFO(ss.str());

			// If you need to log outputs, pass the raw pointer temporarily
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
			AU_CORE_LOG_INFO(ss.str());

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

		// Call with nullptr to get list count.
		output->GetDisplayModeList(format, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC> modeList(count);
		output->GetDisplayModeList(format, flags, &count, &modeList[0]);

		for (auto& x : modeList) {
			UINT n = x.RefreshRate.Numerator;
			UINT d = x.RefreshRate.Denominator;

			std::stringstream ss;
			ss << "Width = " << x.Width << " " << "Height = " << x.Height << " " << "Refresh = " << n << "/" << d;

			//AU_CORE_LOG_INFO(ss.str());
		}
	}
	/*
	void DirectX12Context::UpdateObjectCBs() {
		//auto currObjectCB = m_CurrFrameResource->ObjectCB.get();
		//auto currObjectCB = m_CurrentFrameRenderData->ObjectCB.get();
		for(auto& e : m_AllRitems)
		{
			// Only update the cbuffer data if the constants have changed.  
			// This needs to be tracked per frame resource.
			if(e->NumFramesDirty > 0)
			{
				DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&e->World);

				ObjectConstants objConstants;
				XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

				//currObjectCB->CopyData(e->ObjCBIndex, objConstants);

				// Next FrameResource need to be updated too.
				e->NumFramesDirty--;
			}
		}
	}

	void DirectX12Context::UpdateMainPassCB(Timestep ts) {
		camera.GetCamera().UpdateViewMatrix();
		using namespace DirectX;
		//XMMATRIX view = XMLoadFloat4x4(&m_View);
		XMMATRIX view = camera.GetCamera().GetView();
		//XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
		XMMATRIX proj = camera.GetCamera().GetProj();

		auto t1 = XMMatrixDeterminant(view);
		auto t2 = XMMatrixDeterminant(proj);
		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&t1, view);
		XMMATRIX invProj = XMMatrixInverse(&t2, proj);
		auto t3 = XMMatrixDeterminant(viewProj);
		XMMATRIX invViewProj = XMMatrixInverse(&t3, viewProj);

		XMStoreFloat4x4(&m_MainPassCB.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&m_MainPassCB.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&m_MainPassCB.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&m_MainPassCB.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&m_MainPassCB.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&m_MainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
		//m_MainPassCB.EyePosW = m_EyePos;
		m_MainPassCB.EyePosW = camera.GetCamera().GetPosition3f();
		m_MainPassCB.RenderTargetSize = XMFLOAT2((float)m_WindowHandle->GetWidth(), (float)m_WindowHandle->GetHeight());
		m_MainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / m_WindowHandle->GetWidth(), 1.0f / m_WindowHandle->GetHeight());
		m_MainPassCB.NearZ = 1.0f;
		m_MainPassCB.FarZ = 1000.0f;
		m_MainPassCB.TotalTime = static_cast<float>(Time::GetTime());
		m_MainPassCB.DeltaTime = ts.GetSeconds();
		m_MainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

		XMVECTOR lightDir = -MathHelper::SphericalToCartesian(1.0f, m_SunTheta, m_SunPhi);

		XMStoreFloat3(&m_MainPassCB.Lights[0].Direction, lightDir);
		m_MainPassCB.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };

		//auto currPassCB = m_CurrentFrameRenderData->PassCB.get();
		//currPassCB->CopyData(0, m_MainPassCB);
	}
	*/
	/*
	void DirectX12Context::BuildConstantBufferViews() {
		// --- OBJECT CBV-K (persistent heap) ---
		UINT objCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		UINT matCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(MaterialConstants));
		UINT passCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(PassConstants));

		UINT objCount = (UINT)m_OpaqueRitems.size();
		UINT matCount = (UINT)m_Materials.size();

		UINT descriptorSize = m_HeapManager->GetCbvSrvUavIncrementSize();

		// Persistent CBV tartomány (frameCount * objCount)
		// Ezt valahol előtte kell foglalni:
		// m_ObjectCbvRange = m_HeapManager->AllocateCBV_SRV_UAV_Persistent(m_NumFrameResources * objCount);

		D3D12_CPU_DESCRIPTOR_HANDLE objHandle = m_ObjectCbvRange.cpuBase.handle;

		// Need a CBV descriptor for each object for each frame resource.
		for(uint32_t frameIndex = 0; frameIndex < m_NumFrameResources; ++frameIndex) {
			//auto objectCB = m_FrameResources[frameIndex]->ObjectCB->Resource();
			//auto objectCB = m_FrameRenderData[frameIndex]->ObjectCB->Resource();

			for(UINT i = 0; i < objCount; ++i) {
				//D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress() + i * objCBByteSize;

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
				//cbvDesc.BufferLocation = cbAddress;
				cbvDesc.SizeInBytes = objCBByteSize;

				//m_Device->CreateConstantBufferView(&cbvDesc, objHandle);

				//objHandle.ptr += descriptorSize;
			}
		}

		// --- PASS CBV-K (frame heap) ---
		D3D12_CPU_DESCRIPTOR_HANDLE passHandle = m_PassCbvRange.cpuBase.handle;

		// Last three descriptors are the pass CBVs for each frame resource.
		for(uint32_t frameIndex = 0; frameIndex < m_NumFrameResources; ++frameIndex) {
			//auto passCB = m_FrameResources[frameIndex]->PassCB->Resource();
			//auto passCB = m_FrameRenderData[frameIndex]->PassCB->Resource();

			//D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			//cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = passCBByteSize;

			//m_Device->CreateConstantBufferView(&cbvDesc, passHandle);

			//passHandle.ptr += descriptorSize;
		}

		UINT totalMaterialDescriptors = matCount * m_NumFrameResources;
		D3D12_CPU_DESCRIPTOR_HANDLE matHandle = m_MaterialCbvRange.cpuBase.handle;

		for (uint32_t frameIndex = 0; frameIndex < m_NumFrameResources; ++frameIndex) {
			//auto matCB = m_FrameResources[frameIndex]->MaterialCB->Resource();
			//auto matCB = m_FrameRenderData[frameIndex]->MaterialCB->Resource();

			for (UINT i = 0; i < matCount; ++i) {
				//D3D12_GPU_VIRTUAL_ADDRESS cbAddress = matCB->GetGPUVirtualAddress() + i * matCBByteSize;

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
				//cbvDesc.BufferLocation = cbAddress;
				cbvDesc.SizeInBytes = matCBByteSize;

				//m_Device->CreateConstantBufferView(&cbvDesc, matHandle);

				//matHandle.ptr += descriptorSize;
			}
		}
		
	}
	*/
}
