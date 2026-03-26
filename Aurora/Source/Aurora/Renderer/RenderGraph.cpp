#include "aupch.h"

#include "Aurora/Core/Log.h"
#include "RenderGraph.h"

#include "IRenderPass.h"
#include "Platform/DirectX/Renderer/DirectX12CommandList.h"
#include "Platform/DirectX/Renderer/DirectX12GraphAllocator.h"

namespace Aurora {
	void RenderGraph::SaveStates(IRenderGraphAllocator* allocator) {
		for (const auto& resNode : m_Resources) {
			// we don't care about imported or culled resources
			if (resNode.IsImported || resNode.FirstUsePassIndex == 0xFFFFFFFF) continue;

			auto& physData = m_Registry.Get(resNode.ID);
			if (physData.Resource) {
				allocator->UpdateFinalState(physData.Resource, physData.CurrentState);
			}
		}
	}

	void RenderGraph::Compile(IRenderGraphAllocator* allocator) {
		PerformCulling();

		CalculateLifetimes();

		// allocate with aliasing
		m_Registry.Resize(m_Resources.size());
		AllocatePhysicalResources(allocator);
	}

	void RenderGraph::AllocatePhysicalResources(IRenderGraphAllocator* allocator) {
		m_Registry.Resize(m_Resources.size());

		auto dx12Allocator = dynamic_cast<DirectX12GraphAllocator*>(allocator);
		if (!dx12Allocator) return;

		for (uint32_t passIndex = 0; passIndex < m_Passes.size(); passIndex++) {

			if (passIndex > 0) {
				uint32_t prevPassIndex = passIndex - 1;
				for (const auto& resNode : m_Resources) {
					if (resNode.IsImported || resNode.FirstUsePassIndex == 0xFFFFFFFF) continue;

					// release if we used last in previous pass
					if (resNode.LastUsePassIndex == prevPassIndex) {
						auto& physData = m_Registry.Get(resNode.ID);
						if (resNode.IsTexture) {
							dx12Allocator->ReleaseTexture(resNode.TextureDesc, physData);
						}
					}
				}
			}

			for (const auto& resNode : m_Resources) {
				if (resNode.IsImported || resNode.FirstUsePassIndex == 0xFFFFFFFF) continue;

				if (resNode.FirstUsePassIndex == passIndex) {
					auto& physData = m_Registry.Get(resNode.ID);
					if (resNode.IsTexture) {
						dx12Allocator->AcquireTexture(resNode.TextureDesc, physData);
					} else {
						// TODO: implement buffer allocation for the render graph
						//dx12Allocator->AcquireBuffer()
					}
				}
			}
		}
	}

	void RenderGraph::PerformCulling() {
		// can be improved with proper DAG traversal

		for (auto& pass : m_Passes) {
			pass.IsCulled = true;

			auto checkWrites = [&](const ArenaVector<GraphResourceID>& writes) {
				for (GraphResourceID resID : writes) {
					if (m_Resources[resID].IsImported) {
						pass.IsCulled = false;
					}
				}
			};

			checkWrites(pass.RenderTargetWrites);
			checkWrites(pass.DepthStencilWrites);
			checkWrites(pass.ComputeWrites);
		}

		// backwards propagation - use dependencies too
		for (int i = static_cast<int>(m_Passes.size()) - 1; i >= 0; --i) {
			auto& pass = m_Passes[i];

			if (pass.IsCulled) continue;

			auto resurrectProducer = [&](GraphResourceID resID) {
				uint32_t producerID = m_Resources[resID].ProducerPassID;

				// if it's not imported
				if (producerID != 0xFFFFFFFF) {
					m_Passes[producerID].IsCulled = false;
				}
			};

			for (GraphResourceID resID : pass.TextureReads) resurrectProducer(resID);
			for (GraphResourceID resID : pass.BufferReads) resurrectProducer(resID);
		}
	}

	void RenderGraph::CalculateLifetimes() {
		for (const auto& passNode : m_Passes) {
			if (passNode.IsCulled) continue;

			auto updateLifetime = [&](GraphResourceID resID) {
				if (m_Resources[resID].FirstUsePassIndex == 0xFFFFFFFF) m_Resources[resID].FirstUsePassIndex = passNode.ID;
				m_Resources[resID].LastUsePassIndex = passNode.ID;
			};

			for (auto id : passNode.TextureReads) updateLifetime(id);
			for (auto id : passNode.BufferReads) updateLifetime(id);
			for (auto id : passNode.RenderTargetWrites) updateLifetime(id);
			for (auto id : passNode.DepthStencilWrites) updateLifetime(id);
			for (auto id : passNode.ComputeWrites) updateLifetime(id);
		}
	}

	void RenderGraph::InsertBarriersForPass(IRenderCommandList* cmdList, const PassNode& pass) {
		for (GraphResourceID readID : pass.TextureReads) {
			auto& physData = m_Registry.Get(readID);
			uint32_t neededState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

			if (physData.CurrentState != neededState) {
				ImageBarrier barrier{};
				barrier.ResourceId = readID;
				barrier.OldState = physData.CurrentState;
				barrier.NewState = neededState;

				cmdList->PipelineImageBarrier(barrier);
				physData.CurrentState = neededState;
			}
		}

		for (GraphResourceID rtID : pass.RenderTargetWrites) {
			auto& physData = m_Registry.Get(rtID);
			uint32_t neededState = D3D12_RESOURCE_STATE_RENDER_TARGET;

			if (physData.CurrentState != neededState) {
				ImageBarrier barrier{};
				barrier.ResourceId = rtID;
				barrier.OldState = physData.CurrentState;
				barrier.NewState = neededState;

				cmdList->PipelineImageBarrier(barrier);
				physData.CurrentState = neededState;
			}
		}

		for (GraphResourceID dsID : pass.DepthStencilWrites) {
			auto& physData = m_Registry.Get(dsID);
			uint32_t neededState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

			if (physData.CurrentState != neededState) {
				ImageBarrier barrier{};
				barrier.ResourceId = dsID;
				barrier.OldState = physData.CurrentState;
				barrier.NewState = neededState;

				cmdList->PipelineImageBarrier(barrier);
				physData.CurrentState = neededState;
			}
		}
	}

	GraphResourceID RenderGraph::CreateBuffer(const GraphBufferDesc& desc) {
		// TODO
		return 0;
	}

	void RenderGraph::Execute(IRenderCommandList* cmdList, const SceneData& sceneData) {
		auto dx12CmdList = dynamic_cast<DirectX12CommandList*>(cmdList);
		if (dx12CmdList) dx12CmdList->SetResourceRegistry(&m_Registry);

		for (auto& passNode : m_Passes) {
			if (passNode.IsCulled) continue;

			InsertBarriersForPass(cmdList, passNode);

			passNode.PassInstance->Execute(cmdList, m_Registry, sceneData);
		}
	}

	GraphResourceID RenderGraph::CreateTexture(const GraphTextureDesc& desc) {
		ResourceNode res(m_FrameAllocator);
		res.ID = static_cast<GraphResourceID>(m_Resources.size());
		res.Name = desc.Name;
		res.IsTexture = true;
		res.TextureDesc = desc;
		res.ProducerPassID = m_CurrentPassBuilding;

		m_Resources.push_back(std::move(res));
		return res.ID;
	}

	GraphResourceID RenderGraph::ReadTexture(GraphResourceID id) {
		m_Resources[id].ConsumerPassIDs.push_back(m_CurrentPassBuilding);
		m_Passes[m_CurrentPassBuilding].TextureReads.push_back(id);
		return id;
	}

	GraphResourceID RenderGraph::ReadBuffer(GraphResourceID id) {
		m_Resources[id].ConsumerPassIDs.push_back(m_CurrentPassBuilding);
		m_Passes[m_CurrentPassBuilding].BufferReads.push_back(id);
		return id;
	}

	GraphResourceID RenderGraph::WriteRenderTarget(GraphResourceID id) {
		m_Passes[m_CurrentPassBuilding].RenderTargetWrites.push_back(id);
		return id;
	}

	GraphResourceID RenderGraph::WriteDepthStencil(GraphResourceID id) {
		m_Passes[m_CurrentPassBuilding].DepthStencilWrites.push_back(id);
		return id;
	}

	GraphResourceID RenderGraph::WriteTextureCompute(GraphResourceID id) {
		m_Passes[m_CurrentPassBuilding].ComputeWrites.push_back(id);
		return id;
	}

	GraphResourceID RenderGraph::ImportTexture(std::string_view name, void* physicalResource, uint64_t rtvHandle, uint64_t dsvHandle, uint32_t currentState) {
		ResourceNode res(m_FrameAllocator);
		res.ID = static_cast<GraphResourceID>(m_Resources.size());
		res.Name = name;
		res.IsTexture = true;
		res.IsImported = true;

		// imported resources live forever in terms of the graph
		res.FirstUsePassIndex = 0;
		res.LastUsePassIndex = 0xFFFFFFFE;

		m_Resources.push_back(std::move(res));

		m_Registry.Resize(m_Resources.size());
		auto& physData = m_Registry.Get(res.ID);
		physData.Resource = physicalResource;
		physData.RtvHandlePtr = rtvHandle;
		physData.DsvHandlePtr = dsvHandle;
		physData.CurrentState = currentState;

		return res.ID;
	}

	void RenderGraph::UpdateImportedResource(GraphResourceID id, void* physicalResource, uint64_t rtvHandle, uint64_t dsvHandle) {
		auto& physData = m_Registry.Get(id);
		physData.Resource = physicalResource;
		if (rtvHandle != 0) physData.RtvHandlePtr = rtvHandle;
		if (dsvHandle != 0) physData.DsvHandlePtr = dsvHandle;
	}

	std::string_view RenderGraph::AllocateString(const std::string& str) {
		return m_FrameAllocator.AllocateString(str);
	}
}
