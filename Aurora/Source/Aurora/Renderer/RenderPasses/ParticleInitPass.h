#pragma once

#include "Aurora/Renderer/IRenderPass.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "Platform/DirectX/Renderer/DirectX12CommandList.h"
#include "Platform/DirectX/Renderer/DirectX12RenderCommand.h"

namespace Aurora {
	class ParticleInitPass : public IRenderPass {
	public:
		GraphResourceID DeadListID;
		GraphResourceID CounterID;
		ParticleSystemParams Params;

		ParticleInitPass(GraphResourceID deadList, GraphResourceID counter, const ParticleSystemParams& params)
			: DeadListID(deadList), CounterID(counter), Params(params) {}

		void Setup(IRenderGraphBuilder& builder) override {
			builder.WriteTextureCompute(DeadListID);
			builder.WriteTextureCompute(CounterID);
		}

		void Execute(IRenderCommandList* cmdList, const IRenderGraphResources& resources, const SceneData& sceneData) override {
			//DirectX12CommandList* dx12CmdList = static_cast<DirectX12CommandList*>(cmdList);
			//auto nativeCmdList = dx12CmdList->GetNativeCommandList();
			auto context = DirectX12RenderCommand::GetContext();
			auto nativeCmdList = static_cast<ID3D12GraphicsCommandList*>(cmdList->GetNative());
			auto dx12CmdList = dynamic_cast<DirectX12CommandList*>(cmdList);
			auto rendererAPI = RenderCommand::GetRendererAPIAs<DirectX12RendererAPI>();

			PipelineConfig pConf{};
			pConf.IsCompute = true;
			pConf.ComputeShader = std::static_pointer_cast<DirectX12ComputeShader>(rendererAPI->GetShaderLibrary()->Get("particleInit"));
			auto computePipeline = rendererAPI->GetPipelineLib()->GetOrCreate(pConf, "ParticleInitPSO");

			dx12CmdList->SetComputePipelineState(computePipeline->GetPipelineState());
			dx12CmdList->SetComputeRootSignature(rendererAPI->GetPipelineLib()->GetComputeRootSignature());

			nativeCmdList->SetComputeRoot32BitConstants(0, sizeof(ParticleSystemParams) / 4, &Params, 0);

			ID3D12DescriptorHeap* heaps[] = { rendererAPI->GetTextureManager()->GetBindlessHeap() };
			nativeCmdList->SetDescriptorHeaps(1, heaps);
			nativeCmdList->SetComputeRootDescriptorTable(2, heaps[0]->GetGPUDescriptorHandleForHeapStart());

			//UINT descSize = context->GetHeapManager()->GetCbvSrvUavIncrementSize();

			//auto getGpuHandle = [&](TextureHandle handle) {
			//	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = heaps[0]->GetGPUDescriptorHandleForHeapStart();
			//	gpuHandle.ptr += static_cast<UINT64>(handle.Index) * descSize;
			//	return gpuHandle;
			//};

			//nativeCmdList->SetComputeRootDescriptorTable(2, getGpuHandle(resources.GetUAVHandle(DeadListID)));
			//nativeCmdList->SetComputeRootDescriptorTable(3, getGpuHandle(resources.GetUAVHandle(CounterID)));

			// dispatch
			uint32_t threadGroups = (Params.MaxParticles + 255) / 256;
			dx12CmdList->Dispatch(threadGroups, 1, 1);
		}
	};
}
