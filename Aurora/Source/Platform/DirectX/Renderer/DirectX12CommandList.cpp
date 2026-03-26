#include "aupch.h"

#include "DirectX12CommandList.h"

namespace Aurora {
	DirectX12CommandList::DirectX12CommandList(DirectX12Context* context) : m_Context(context) {
		m_CmdList = context->GetCommandList();
	}

	void DirectX12CommandList::Begin() {

	}

	void DirectX12CommandList::End() {

	}

	void DirectX12CommandList::SetPipeline(std::shared_ptr<Pipeline> pipeline) {
	}

	void DirectX12CommandList::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) {
		D3D12_VIEWPORT vp = { x, y, width, height, minDepth, maxDepth };
		m_CmdList->RSSetViewports(1, &vp);
	}

	void DirectX12CommandList::SetScissor(int x, int y, int width, int height) {
		D3D12_RECT rect = {
			static_cast<LONG>(x),
			static_cast<LONG>(y),
			static_cast<LONG>(x + width),
			static_cast<LONG>(y + height)
		};
		m_CmdList->RSSetScissorRects(1, &rect);
	}

	void DirectX12CommandList::SetVertexBuffer(uint32_t slot, std::shared_ptr<Buffer> buffer, uint64_t offset, uint32_t stride) {
	}

	void DirectX12CommandList::SetIndexBuffer(std::shared_ptr<Buffer> buffer, uint64_t offset, IndexFormat format) {
	}

	void DirectX12CommandList::BindConstants(uint32_t slot, const void* data, size_t size) {
	}

	void DirectX12CommandList::BindDescriptorSet(uint32_t slot, DescriptorSet* descriptorSet) {
	}

	void DirectX12CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
		uint32_t firstInstance) {
	}

	void DirectX12CommandList::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
		uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) {

		m_CmdList->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	void DirectX12CommandList::BeginRendering(const RenderPassInfo& renderPassInfo) {
		if (!m_ResourceRegistry) return;

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
		rtvHandles.reserve(renderPassInfo.ColorAttachments.size());

		for (const auto& attachment : renderPassInfo.ColorAttachments) {
			if (attachment.ResourceId != INVALID_RESOURCE_ID) {
				D3D12_CPU_DESCRIPTOR_HANDLE rtv = { m_ResourceRegistry->GetRtvHandlePtr(attachment.ResourceId) };
				rtvHandles.push_back(rtv);

				if (attachment.LoadAction == LoadOp::Clear) {
					m_CmdList->ClearRenderTargetView(rtv, attachment.ClearColor, 0, nullptr);
				}
			}
		}

		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};
		const D3D12_CPU_DESCRIPTOR_HANDLE* pDsvHandle = nullptr;

		if (renderPassInfo.HasDepth && renderPassInfo.DepthAttachment.ResourceId != INVALID_RESOURCE_ID) {
			dsvHandle = { m_ResourceRegistry->GetDsvHandlePtr(renderPassInfo.DepthAttachment.ResourceId) };
			pDsvHandle = &dsvHandle;

			if (renderPassInfo.DepthAttachment.LoadAction == LoadOp::Clear) {
				//m_CmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
				m_CmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);
			}
		}

		m_CmdList->OMSetRenderTargets(
			static_cast<UINT>(rtvHandles.size()),
			rtvHandles.empty() ? nullptr : rtvHandles.data(),
			FALSE,
			pDsvHandle
		);
	}

	void DirectX12CommandList::EndRendering() {
		// for more modern solution use BeginRenderPass and EndRenderPass instead of OMSetRenderTargets
	}

	void DirectX12CommandList::PipelineImageBarrier(const ImageBarrier& barrier) {
		if (!m_ResourceRegistry) return;

		ID3D12Resource* physicalResource = static_cast<ID3D12Resource*>(m_ResourceRegistry->GetPhysicalResource(barrier.ResourceId));
		if (!physicalResource) return;

		D3D12_RESOURCE_BARRIER d3dBarrier = {};
		d3dBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dBarrier.Transition.pResource = physicalResource;
		d3dBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		d3dBarrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(barrier.OldState);
		d3dBarrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(barrier.NewState);

		m_CmdList->ResourceBarrier(1, &d3dBarrier);
	}
}
