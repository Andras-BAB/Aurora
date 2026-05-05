#pragma once
#include "IRenderPass.h"
#include "ParticleSystem.h"
#include "Platform/DirectX/Renderer/DirectX12CommandList.h"
#include "Platform/DirectX/Renderer/DirectX12RenderCommand.h"

namespace Aurora {
	class ParticleUpdatePass : public IRenderPass {
	public:
		GraphResourceID ParticleID, DeadListID, CounterID;
		ParticleSystemParams Params;
		uint32_t MaxParticles;

		ParticleUpdatePass(GraphResourceID p, GraphResourceID dl, GraphResourceID c, const ParticleSystemParams& params, uint32_t maxP)
			: ParticleID(p), DeadListID(dl), CounterID(c), Params(params), MaxParticles(maxP) {}

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
			pConf.ComputeShader = std::static_pointer_cast<DirectX12ComputeShader>(rendererAPI->GetShaderLibrary()->Get("particleUpdate"));
			auto computePipeline = rendererAPI->GetPipelineLib()->GetOrCreate(pConf, "ParticleUpdatePSO");

			dx12CmdList->SetComputePipelineState(computePipeline->GetPipelineState());
			dx12CmdList->SetComputeRootSignature(rendererAPI->GetPipelineLib()->GetComputeRootSignature());

			nativeCmdList->SetComputeRoot32BitConstants(0, sizeof(ParticleSystemParams) / 4, &Params, 0);

			// setting the bindless table
			ID3D12DescriptorHeap* heaps[] = { rendererAPI->GetTextureManager()->GetBindlessHeap() };
			nativeCmdList->SetDescriptorHeaps(1, heaps);
			nativeCmdList->SetComputeRootDescriptorTable(2, heaps[0]->GetGPUDescriptorHandleForHeapStart());

			// update by 256 threads
			uint32_t threadGroups = (MaxParticles + 255) / 256;
			dx12CmdList->Dispatch(threadGroups, 1, 1);
		}
	};
}
