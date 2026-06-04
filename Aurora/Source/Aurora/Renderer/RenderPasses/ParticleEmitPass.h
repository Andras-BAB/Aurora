#pragma once

#include "Aurora/Renderer/IRenderPass.h"
#include "Aurora/Renderer/ParticleSystem.h"

#include "Platform/DirectX/Renderer/DirectX12CommandList.h"
#include "Platform/DirectX/Renderer/DirectX12RenderCommand.h"

namespace Aurora {
	class ParticleEmitPass : public IRenderPass {
	public:
		GraphResourceID ParticleID, DeadListID, CounterID;
		ParticleSystemParams Params;
		uint32_t EmitCount;

		ParticleEmitPass(GraphResourceID p, GraphResourceID dl, GraphResourceID c, const ParticleSystemParams& params, uint32_t count)
			: ParticleID(p), DeadListID(dl), CounterID(c), Params(params), EmitCount(count) {}

		void Setup(IRenderGraphBuilder& builder) override {
			builder.WriteTextureCompute(ParticleID);
			builder.WriteTextureCompute(DeadListID);
			builder.WriteTextureCompute(CounterID);
		}

		void Execute(IRenderCommandList* cmdList, const IRenderGraphResources& resources, const SceneData& sceneData) override {
			auto context = DirectX12RenderCommand::GetContext();
			auto nativeCmdList = static_cast<ID3D12GraphicsCommandList*>(cmdList->GetNative());
			auto dx12CmdList = dynamic_cast<DirectX12CommandList*>(cmdList);
			auto rendererAPI = RenderCommand::GetRendererAPIAs<DirectX12RendererAPI>();

			PipelineConfig pConf{};
			pConf.IsCompute = true;
			pConf.ComputeShader = std::static_pointer_cast<DirectX12ComputeShader>(rendererAPI->GetShaderLibrary()->Get("particleEmit"));
			auto computePipeline = rendererAPI->GetPipelineLib()->GetOrCreate(pConf, "ParticleEmitPSO");

			dx12CmdList->SetComputePipelineState(computePipeline->GetPipelineState());
			dx12CmdList->SetComputeRootSignature(rendererAPI->GetPipelineLib()->GetComputeRootSignature());

			nativeCmdList->SetComputeRoot32BitConstants(0, sizeof(ParticleSystemParams) / 4, &Params, 0);

			ID3D12DescriptorHeap* heaps[] = { rendererAPI->GetTextureManager()->GetBindlessHeap() };
			nativeCmdList->SetDescriptorHeaps(1, heaps);
			nativeCmdList->SetComputeRootDescriptorTable(2, heaps[0]->GetGPUDescriptorHandleForHeapStart());

			// there are 64 threads in a group
			uint32_t threadGroups = (EmitCount + 63) / 64;
			dx12CmdList->Dispatch(threadGroups, 1, 1);
		}
	};
}
