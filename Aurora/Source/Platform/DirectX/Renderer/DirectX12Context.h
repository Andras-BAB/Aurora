#pragma once
#include "Aurora/Renderer/GraphicsContext.h"
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

//inline const int g_NumFrameResources = 3;

namespace Aurora {
		
	struct RenderItem {
		RenderItem() = default;

		// World matrix of the shape that describes the object's local space
		// relative to the world space, which defines the position, orientation,
		// and scale of the object in the world.
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();

		// Dirty flag indicating the object data has changed and we need to update the constant buffer.
		// Because we have an object cbuffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify object data we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		//int NumFramesDirty = g_NumFrameResources;
		int NumFramesDirty = 0;

		// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
		UINT ObjCBIndex = -1;

		d3dUtil::MeshGeometry* Geo = nullptr;
		d3dUtil::Material* Mat = nullptr;

		// Primitive topology.
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// DrawIndexedInstanced parameters.
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	enum class RenderLayer : int {
		Opaque = 0,
		Count
	};

	class DirectX12Context : public GraphicsContext {
	public:
		DirectX12Context(WindowsWindow* windowHandle);
		~DirectX12Context() override = default;
		DirectX12Context(const DirectX12Context&) = delete;
		DirectX12Context(DirectX12Context&&) = delete;
		DirectX12Context operator=(DirectX12Context) = delete;
		DirectX12Context operator=(DirectX12Context&&) = delete;

		void Init() override;
		void Shutdown() override;
		void SwapBuffers(Timestep ts) override;
		
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
		
	private:
		friend class ImGuiLayer;
		friend class DirectX12RendererAPI;

		void SetRenderTarget();
		
		void LogAdapters() const;
		void LogAdapterOutputs(IDXGIAdapter* adapter) const;
		static void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

		//void UpdateCamera(Timestep ts);
		//void UpdateObjectCBs();
		//void UpdateMainPassCB(Timestep ts);
		
		void CreateCommandObjects();
		void CreateSwapChain();
		void BuildDescriptorHeaps();
		//void BuildConstantBufferViews();

		//void BuildFrameResources();
		//void BuildRenderItems();
		//void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

		void FlushCommandQueue();
		
		//void BuildShapeGeometry();
		//void BuildMaterials();
		//void UpdateMaterialCBs();

		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

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

		//std::vector<std::unique_ptr<FrameRenderData>> m_FrameRenderData;
		//FrameRenderData* m_CurrentFrameRenderData = nullptr;

		DirectX12SwapChain m_SwapChain;

		std::unique_ptr<DirectX12HeapManager> m_HeapManager;
		//DescriptorRange m_PassCbvRange;
		//DescriptorRange m_ObjectCbvRange;
		//DescriptorRange m_MaterialCbvRange;

		//std::unordered_map<std::string, std::unique_ptr<d3dUtil::MeshGeometry>> m_Geometries;
		//std::unordered_map<std::string, std::unique_ptr<d3dUtil::Material>> m_Materials;
		
		// List of all the render items.
		//std::vector<std::unique_ptr<RenderItem>> m_AllRitems;
		// Render items divided by PSO.
		//std::vector<RenderItem*> m_OpaqueRitems;

		//PassConstants m_MainPassCB;
		//UINT m_PassCbvOffset = 0;

		//float m_SunTheta = 1.25f * DirectX::XM_PI;
		//float m_SunPhi = DirectX::XM_PIDIV4;

		//DirectX12PipelineStateLibrary m_PipelineLib;

		//PerspectiveCameraController camera;
		//bool isFirstFrame = true;

	};
	
}
