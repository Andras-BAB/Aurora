#pragma once
#include "DirectX12HeapManager.h"
#include "Platform/DirectX/Utils/MSUtils.h"

namespace Aurora {
	class DirectX12Context;

	class DirectX12SwapChain {
	public:
		DirectX12SwapChain() = default;
		DirectX12SwapChain(DirectX12SwapChain&) = delete;
		DirectX12SwapChain(DirectX12SwapChain&&) = delete;
		DirectX12SwapChain& operator=(DirectX12SwapChain&) = delete;
		DirectX12SwapChain& operator=(DirectX12SwapChain&&) = delete;
		~DirectX12SwapChain() = default;

		void Init(IDXGIFactory6* factory, ID3D12CommandQueue* commandQueue, HWND hWnd, UINT width, UINT height,
			DirectX12HeapManager* heapManager);
		void Shutdown();
		void Present();
		void Resize(UINT width, UINT height, ID3D12Device* device, DirectX12HeapManager* heapManager, ID3D12GraphicsCommandList* currentCmdList);
		void SetVSync(bool vsync);
		bool IsVSync() const;

		DXGI_FORMAT GetBackBufferFormat() const;
		DXGI_FORMAT GetDepthStencilFormat() const;
		ID3D12Resource* GetCurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
		uint32_t CurrentBackBufferIndex() const;
		
	private:
		uint32_t m_BufferCount = 3;
		bool m_4xMsaaState = false;
		UINT m_4xMsaaQuality = 0;
		
		MS::ComPtr<IDXGISwapChain4> m_SwapChain;
		DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		std::vector<MS::ComPtr<ID3D12Resource>> m_Buffers;
		uint32_t m_CurrentBackBuffer = 0;
		MS::ComPtr<ID3D12Resource> m_DepthStencilBuffer;

		DescriptorRange m_RtvHandle;
		DescriptorRange m_DsvHandle;
		UINT m_RtvHandleSize;
		UINT m_DsvHandleSize;

		bool m_VSync = false;

		friend class DirectX12Context;
	};
}
