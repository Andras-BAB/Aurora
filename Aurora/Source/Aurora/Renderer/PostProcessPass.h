#pragma once

#include "IRenderPass.h"
#include "RenderCommand.h"

#include "Platform/DirectX/Renderer/DirectX12CommandList.h"

namespace Aurora {
	class PostProcessPass : public IRenderPass {
	public:
		GraphResourceID InputTextureID = INVALID_RESOURCE_ID;
		GraphResourceID OutputTargetID = INVALID_RESOURCE_ID;

		PostProcessPass(GraphResourceID input, GraphResourceID output)
			: InputTextureID(input), OutputTargetID(output) {
		}

		void Setup(IRenderGraphBuilder& builder) override {
			if (InputTextureID != INVALID_RESOURCE_ID) builder.ReadTexture(InputTextureID);
			if (OutputTargetID != INVALID_RESOURCE_ID) builder.WriteRenderTarget(OutputTargetID);
		}

		void Execute(IRenderCommandList* cmdList, const IRenderGraphResources& resources, const SceneData& sceneData) override {
			RenderPassInfo passInfo{};
			if (OutputTargetID != INVALID_RESOURCE_ID) {
				RenderAttachment outAttachment{};
				outAttachment.ResourceId = OutputTargetID;
				outAttachment.LoadAction = LoadOp::DontCare;
				passInfo.ColorAttachments.push_back(outAttachment);
			}

			cmdList->BeginRendering(passInfo);

			if (auto nativeCmdList = static_cast<ID3D12GraphicsCommandList*>(cmdList->GetNative())) {
				auto context = RenderCommand::GetContextAs<DirectX12Context>();
				auto device = context->GetDevice();
				auto rendererAPI = RenderCommand::GetRendererAPIAs<DirectX12RendererAPI>();

				PipelineConfig pConf{};
				pConf.VertexShader = std::static_pointer_cast<DirectX12VertexShader>(rendererAPI->GetShaderLibrary()->Get("postProcessVert"));
				pConf.PixelShader = std::static_pointer_cast<DirectX12PixelShader>(rendererAPI->GetShaderLibrary()->Get("postProcessPixel"));
				pConf.InputLayout = {};
				pConf.Depth = DepthMode::None;
				auto ppPipeline = rendererAPI->GetPipelineLib()->GetOrCreate(pConf);

				nativeCmdList->SetGraphicsRootSignature(rendererAPI->GetPipelineLib()->GetUberRootSignature());
				nativeCmdList->SetPipelineState(ppPipeline->GetPipelineState());

				ID3D12DescriptorHeap* heaps[] = { rendererAPI->GetTextureManager()->GetBindlessHeap() };
				nativeCmdList->SetDescriptorHeaps(1, heaps);

				TextureHandle srvHandle = resources.GetTextureHandle(InputTextureID);
				UINT descriptorSize = context->GetHeapManager()->GetCbvSrvUavIncrementSize();

				D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = heaps[0]->GetGPUDescriptorHandleForHeapStart();
				gpuHandle.ptr += static_cast<UINT64>(srvHandle.Index) * descriptorSize;

				nativeCmdList->SetGraphicsRootDescriptorTable(3, gpuHandle);

				nativeCmdList->IASetVertexBuffers(0, 0, nullptr);
				nativeCmdList->IASetIndexBuffer(nullptr);
				nativeCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				nativeCmdList->DrawInstanced(3, 1, 0, 0);
			}

			cmdList->EndRendering();
		}
	};
}