#pragma once
#include "Aurora/Renderer/IGraphicsContext.h"
#include "Platform/DirectX/Utils/MSUtils.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include "DirectX12HeapManager.h"
#include "DirectX12PipelineState.h"
#include "DirectX12SwapChain.h"
#include "Platform/Windows/WindowsWindow.h"
#include "UploadBuffer.h"
#include "Platform/DirectX/Renderer/FrameResource.h"
#include "Aurora/Renderer/PerspectiveCameraController.h"

namespace Aurora {

	class DirectX12Context : public IGraphicsContext {
	public:
		DirectX12Context(WindowsWindow* windowHandle);
		~DirectX12Context() override = default;
		DirectX12Context(const DirectX12Context&) = delete;
		DirectX12Context(DirectX12Context&&) = delete;
		DirectX12Context operator=(DirectX12Context) = delete;
		DirectX12Context operator=(DirectX12Context&&) = delete;

		void Init() override;
		void Shutdown() override;
		void SwapBuffers() override;
		
		void BeginFrame() override;
		void SubmitFrame() override;
		
		void OnWindowResize() override;

		ID3D12Device* GetDevice() const;
		ID3D12GraphicsCommandList* GetCommandList() const;
		void SubmitCommandList() const;
		void WaitForFence(UINT64 fence) const;
		void SignalQueue() const;

		uint32_t GetCurrentFrameSyncIndex() const;
		uint32_t GetFrameResourcesCount() const;
		ID3D12CommandQueue* GetCommandQueue() const;

		DirectX12HeapManager* GetHeapManager() const;
		
		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

		void CreateCommandObjects();
		void CreateSwapChain();

		void FlushCommandQueue();

	private:
		friend class ImGuiLayer;
		friend class DirectX12RendererAPI;

		void SetRenderTarget();
		
		void LogAdapters() const;
		void LogAdapterOutputs(IDXGIAdapter* adapter) const;
		static void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	private:
		WindowsWindow* m_WindowHandle;
		uint32_t m_SwapChainBufferCount = 3;
		uint32_t m_NumFrameResources = 3;
		
		MS::ComPtr<ID3D12Device> m_Device;
		MS::ComPtr<IDXGIFactory6> m_DxgiFactory;

		MS::ComPtr<ID3D12Fence> m_Fence;
		UINT64 m_CurrentFence = 0;

		UINT m_4xMsaaQuality = 0;
		BOOL m_4xMsaaState = false; // For msaa render to another buffer, then use ResolveSubresource with backbuffer dest

		MS::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		MS::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		
		std::vector<std::unique_ptr<FrameSync>> m_FrameSyncs;
		uint32_t m_CurrentFrameSyncIndex = 0;

		DirectX12SwapChain m_SwapChain;

		std::unique_ptr<DirectX12HeapManager> m_HeapManager;
	};
	
}
