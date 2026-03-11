#pragma once
#include "DirectX12Buffer.h"
#include "DirectX12Context.h"
#include "DirectX12TextureManager.h"
#include "Aurora/Renderer/RendererAPI.h"
#include "Aurora/Scene/MaterialManager.h"

namespace Aurora {

	class DirectX12RendererAPI : public RendererAPI {
	public:
		DirectX12RendererAPI();
		~DirectX12RendererAPI() override = default;

		void Init() override;
		void Shutdown() override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void ResizeSwapchainImage(uint32_t width, uint32_t height) override;
		void SetClearColor(const DirectX::XMFLOAT4& color) override;
		void Clear() override;

		//void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0);
		//void DrawIndexed(const std::shared_ptr<MeshAsset>& meshAsset) override;
		void DrawIndexed(const std::shared_ptr<d3dUtil::MeshGeometry>& meshGeo) override;
		MeshAllocation CreateMesh(const MeshData& meshData) override;
		
		void SetLineWidth(float width) override;

		// TODO: RendererAPI don't need to know anything about Entity
		void SubmitEntity(Entity entity) override;

		void SubmitProxy(const RenderProxyData& proxyData) override;
		void DeleteRenderProxy(uint32_t entityID, uint32_t submeshCount);

		void RenderActiveList(ID3D12GraphicsCommandList* cmdList);

		void BeginFrame(const SceneData& sceneData) override;
		void EndFrame() override;

		void SetContext(GraphicsContext* context) override;
		GraphicsContext* GetContext() const override;

	private:
		struct DirectX12RenderProxy {
			uint32_t IndexCount = 0;
			uint32_t StartIndexLocation = 0;	// index offset, not byte (divided by stride)
			uint32_t BaseVertexLocation = 0;	// vertex offset, not byte (divided by stride)

			UINT ObjCBIndex = -1;				// Az UploadBuffer-beli index (a CopyData-hoz)
			UINT MatCBIndex = -1;
			int NumFramesDirty = 0;

			DescriptorRange ObjCBRange;			// A Descriptor Heap tartomány (a SetGraphicsRoot-hoz)
			DescriptorRange MatCBRange;

			DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();

			//d3dUtil::Material* Material = nullptr;
			//std::shared_ptr<MaterialAsset> Material;
		};
	
	private:
		void SetViewport() const;
		void SetScissors() const;

		void UpdateConstantBuffers();
		void UpdatePassCBV(uint32_t frameIndex);
		void CreateProxyCBVs(DirectX12RenderProxy& proxy, uint32_t objIndex, uint32_t matIndex);
		void CommitMeshes(ID3D12GraphicsCommandList* cmdList);

		void EnsureCapacity(uint32_t neededObjCount, uint32_t neededMatCount);
		
	private:
		DirectX12Context* m_Context = nullptr;

		DirectX::XMFLOAT4 m_ClearColor = { 0.5f, 0.5f, 0.5f, 1.0f };

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		std::vector<std::unique_ptr<FrameRenderData>> m_FrameData;
		FrameRenderData* m_CurrentFrameData = nullptr;

		DescriptorRange m_PassCBVRange;

		std::unique_ptr<GlobalMeshBuffer> m_GlobalMeshBuffer;
		//std::vector<MeshData> m_PendingMeshes;
		struct PendingMesh {
			std::vector<uint8_t> VertexData;
			std::vector<uint8_t> IndexData;
			MeshAllocation allocation;
			uint32_t VertexStride;
		};
		std::vector<PendingMesh> m_PendingMeshes;

		std::unordered_map<uint64_t, DirectX12RenderProxy> m_ProxyCache;

		MaterialManager m_MaterialManager;
		std::unique_ptr<DirectX12TextureManager> m_TextureManager;

		//std::unordered_map<MeshAsset*, std::unique_ptr<d3dUtil::MeshGeometry>> m_MeshCache;

		std::vector<DirectX12RenderProxy*> m_ActiveDrawList;
		D3D12_RESOURCE_STATES m_VbState = D3D12_RESOURCE_STATE_COMMON;
		D3D12_RESOURCE_STATES m_IbState = D3D12_RESOURCE_STATE_COMMON;

		uint32_t m_ObjectCBCapacity = 128;
		uint32_t m_MaterialCBCapacity = 128;

		PassConstants m_MainPassCb;
		float m_SunTheta = 1.25f * DirectX::XM_PI;
		float m_SunPhi = DirectX::XM_PIDIV4;

		DirectX12PipelineStateLibrary m_PipelineLib;
	};
}
