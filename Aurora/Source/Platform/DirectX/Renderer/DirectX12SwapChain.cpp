#include "aupch.h"
#include "DirectX12SwapChain.h"

#include "Platform/Windows/WindowsWindow.h"
#include "DirectX12Context.h"

namespace Aurora {

	void DirectX12SwapChain::Init(IDXGIFactory6* factory, ID3D12CommandQueue* commandQueue, HWND hWnd, UINT width, UINT height,
		DirectX12HeapManager* heapManager) {
		
		m_SwapChain.Reset();
		m_Buffers.resize(m_BufferCount);

		MS::ComPtr<IDXGISwapChain1> swapChain1;
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = width;
		sd.Height = height;
		sd.Format = m_BackBufferFormat;
		sd.Stereo = FALSE;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = m_BufferCount;
		sd.Scaling = DXGI_SCALING_NONE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
		sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		sd.Flags = 0;

		ThrowOnFail(factory->CreateSwapChainForHwnd(
			commandQueue,
			hWnd,
			&sd,
			nullptr,
			nullptr,
			swapChain1.GetAddressOf())
		);

		ThrowOnFail(swapChain1.As(&m_SwapChain));

		m_RtvHandle = heapManager->AllocateRTV(m_BufferCount);
		m_DsvHandle = heapManager->AllocateDSV();

		m_RtvHandleSize = heapManager->GetRtvHandleIncrementSize();
		m_DsvHandleSize = heapManager->GetDsvHandleIncrementSize();
	}

	void DirectX12SwapChain::Shutdown() {
		for (uint32_t i = 0; i < m_BufferCount; ++i) {
			m_Buffers[i].Reset();
		}
		m_DepthStencilBuffer.Reset();
	}

	void DirectX12SwapChain::Present() {
		ThrowOnFail(m_SwapChain->Present(m_VSync, 0));
		m_CurrentBackBuffer = (m_CurrentBackBuffer + 1) % m_BufferCount;
	}

	void DirectX12SwapChain::Resize(UINT width, UINT height, ID3D12Device* device, 
		DirectX12HeapManager* heapManager, ID3D12GraphicsCommandList* currentCmdList) {

		// Release the previous resources we will be recreating.
		for (uint32_t i = 0; i < m_BufferCount; ++i) {
			m_Buffers[i].Reset();
		}
		m_DepthStencilBuffer.Reset();
		
		// Resize the swap chain.
		ThrowOnFail(m_SwapChain->ResizeBuffers(
			m_BufferCount,
			width, height,
			m_BackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
		
		m_CurrentBackBuffer = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RtvHandle.cpuBase.handle;

		for (UINT i = 0; i < m_BufferCount; i++) {
			ThrowOnFail(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_Buffers[i])));
			device->CreateRenderTargetView(m_Buffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += heapManager->GetRtvHandleIncrementSize();
		}
		
		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = m_DepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		D3D12_HEAP_PROPERTIES props = {};
		props.Type = D3D12_HEAP_TYPE_DEFAULT;
		props.CreationNodeMask = 1;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		props.VisibleNodeMask = 1;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		ThrowOnFail(device->CreateCommittedResource(
			&props,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = m_DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, m_DsvHandle.cpuBase.handle);


		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_DepthStencilBuffer.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		// Transition the resource from its initial state to be used as a depth buffer.
		currentCmdList->ResourceBarrier(1, &barrier);
	}

	void DirectX12SwapChain::SetVSync(const bool vsync) {
		m_VSync = vsync;
	}

	bool DirectX12SwapChain::IsVSync() const {
		return m_VSync;
	}

	DXGI_FORMAT DirectX12SwapChain::GetBackBufferFormat() const {
		return m_BackBufferFormat;
	}

	DXGI_FORMAT DirectX12SwapChain::GetDepthStencilFormat() const {
		return m_DepthStencilFormat;
	}

	ID3D12Resource* DirectX12SwapChain::GetCurrentBackBuffer() const {
		return m_Buffers[m_CurrentBackBuffer].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12SwapChain::GetCurrentBackBufferView() const {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RtvHandle.cpuBase.handle;
		handle.ptr += static_cast<SIZE_T>(m_CurrentBackBuffer) * m_RtvHandleSize;
		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12SwapChain::GetDepthStencilView() const {
		return m_DsvHandle.cpuBase.handle;
	}

	uint32_t DirectX12SwapChain::CurrentBackBufferIndex() const {
		return m_CurrentBackBuffer;
	}
}
