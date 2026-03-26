#pragma once

#include "Aurora/Renderer/IRenderCommandList.h"
#include "Platform/DirectX/Renderer/DirectX12Context.h"
#include <d3d12.h>
#include <wrl.h>
#include <vector>

namespace Aurora {

	class DirectX12CommandList : public IRenderCommandList {
	public:
		DirectX12CommandList(DirectX12Context* context);
		~DirectX12CommandList() override = default;

		void SetResourceRegistry(const IRenderGraphResources* registry) { m_ResourceRegistry = registry; }

		void Begin() override;
		void End() override;

		void SetPipeline(std::shared_ptr<Pipeline> pipeline) override;
		void SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) override;
		void SetScissor(int x, int y, int width, int height) override;

		void SetVertexBuffer(uint32_t slot, std::shared_ptr<Buffer> buffer, uint64_t offset = 0, uint32_t stride = 0) override;
		void SetIndexBuffer(std::shared_ptr<Buffer> buffer, uint64_t offset = 0, IndexFormat format = IndexFormat::Uint32) override;

		void BindConstants(uint32_t slot, const void* data, size_t size) override;
		void BindDescriptorSet(uint32_t slot, DescriptorSet* descriptorSet) override;

		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) override;

		void BeginRendering(const RenderPassInfo& renderPassInfo) override;
		void EndRendering() override;
		void PipelineImageBarrier(const ImageBarrier& barrier) override;

		void* GetNative() const override { return (void*)m_CmdList; }

	private:
		// TODO: this has to own the command list
		DirectX12Context* m_Context = nullptr;
		ID3D12GraphicsCommandList* m_CmdList = nullptr;

		// where are the GraphResourceIDs in physical memory
		const IRenderGraphResources* m_ResourceRegistry = nullptr;
	};

}
