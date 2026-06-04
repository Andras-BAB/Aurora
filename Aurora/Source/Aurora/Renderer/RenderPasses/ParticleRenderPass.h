#pragma once

#include "Aurora/Renderer/IRenderPass.h"
#include "Aurora/Renderer/RenderCommand.h"

#include "Aurora/Scene/Components.h"
#include "Aurora/Math/Math.h"

#include "Platform/DirectX/Renderer/DirectX12CommandList.h"

namespace Aurora {

	class ParticleRenderPass : public IRenderPass {
	public:
		GraphResourceID ParticleBufferID;
		GraphResourceID ColorTargetID;
		GraphResourceID DepthTargetID;
		//uint32_t MaxParticles;

		const ParticleEmitterComponent* EmitterParams = nullptr;

		//ParticleRenderPass(GraphResourceID particleBuffer, GraphResourceID colorTarget, GraphResourceID depthTarget, uint32_t maxParticles)
		//	: ParticleBufferID(particleBuffer), ColorTargetID(colorTarget), DepthTargetID(depthTarget), MaxParticles(maxParticles) {}

		ParticleRenderPass(GraphResourceID particleBuffer, GraphResourceID colorTarget, GraphResourceID depthTarget, const ParticleEmitterComponent* params)
			: ParticleBufferID(particleBuffer), ColorTargetID(colorTarget), DepthTargetID(depthTarget), EmitterParams(params) {}

		void Setup(IRenderGraphBuilder& builder) override {
			builder.ReadBuffer(ParticleBufferID);
			builder.WriteRenderTarget(ColorTargetID);
			//builder.WriteDepthStencil(DepthTargetID);
			builder.ReadTexture(DepthTargetID);
		}

		void Execute(IRenderCommandList* cmdList, const IRenderGraphResources& resources, const SceneData& sceneData) override {
			RenderPassInfo passInfo{};

			RenderAttachment outAttachment{};
			outAttachment.ResourceId = ColorTargetID;
			outAttachment.LoadAction = LoadOp::Load;
			passInfo.ColorAttachments.push_back(outAttachment);

			//if (DepthTargetID != INVALID_RESOURCE_ID) {
			//	passInfo.DepthAttachment.ResourceId = DepthTargetID;
			//	passInfo.DepthAttachment.LoadAction = LoadOp::Load;
			//	passInfo.HasDepth = true;
			//}

			cmdList->BeginRendering(passInfo);

			auto nativeCmdList = static_cast<ID3D12GraphicsCommandList*>(cmdList->GetNative());
			auto dx12CmdList = dynamic_cast<DirectX12CommandList*>(cmdList);
			auto rendererAPI = RenderCommand::GetRendererAPIAs<DirectX12RendererAPI>();
			auto context = RenderCommand::GetContextAs<DirectX12Context>();

			PipelineConfig pConf{};
			pConf.VertexShader = std::static_pointer_cast<DirectX12VertexShader>(rendererAPI->GetShaderLibrary()->Get("particleVert"));
			pConf.PixelShader = std::static_pointer_cast<DirectX12PixelShader>(rendererAPI->GetShaderLibrary()->Get("particlePixel"));
			pConf.InputLayout = {};
			pConf.Topology = TopologyType::Triangle;
			pConf.Depth = DepthMode::None;
			pConf.DepthStencilFormat = DXGI_FORMAT_UNKNOWN;
			pConf.BackBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
			pConf.Blend = EmitterParams->BlendMode;
			pConf.Cull = CullMode::None;
			pConf.SampleCount = 4;
			pConf.SampleQuality = 0;

			auto pipeline = rendererAPI->GetPipelineLib()->GetOrCreate(pConf, "ParticleRenderPSO");
			nativeCmdList->SetGraphicsRootSignature(rendererAPI->GetPipelineLib()->GetUberRootSignature());
			nativeCmdList->SetPipelineState(pipeline->GetPipelineState());


			ID3D12DescriptorHeap* heaps[] = { rendererAPI->GetTextureManager()->GetBindlessHeap() };
			nativeCmdList->SetDescriptorHeaps(1, heaps);

			D3D12_GPU_DESCRIPTOR_HANDLE heapStart = heaps[0]->GetGPUDescriptorHandleForHeapStart();
			nativeCmdList->SetGraphicsRootDescriptorTable(3, heapStart);

			TextureHandle particleHandle = resources.GetTextureHandle(ParticleBufferID);
			TextureHandle depthHandle = resources.GetTextureHandle(DepthTargetID);
			uint32_t gradientIndex = EmitterParams->GradientTexture ? EmitterParams->GradientTexture->GetHandle().Index : 0;
			uint32_t textureIndex = EmitterParams->Texture ? EmitterParams->Texture->GetHandle().Index : 0;

			uint32_t indices[4] = { particleHandle.Index, depthHandle.Index, gradientIndex, textureIndex };
			nativeCmdList->SetGraphicsRoot32BitConstants(4, _countof(indices), indices, 0);

			// pass constants
			UINT passCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(PassConstants));
			D3D12_GPU_VIRTUAL_ADDRESS passAddress = rendererAPI->m_CurrentFrameData->PassCB->Resource()->GetGPUVirtualAddress();
			passAddress += static_cast<UINT64>(sceneData.MainView.ViewID) * passCBByteSize;

			nativeCmdList->SetGraphicsRootConstantBufferView(0, passAddress);

			// there are no vertex and index buffers, generated in shaders
			nativeCmdList->IASetVertexBuffers(0, 0, nullptr);
			nativeCmdList->IASetIndexBuffer(nullptr);
			nativeCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			// 4 vertices for each particle
			nativeCmdList->DrawInstanced(4, EmitterParams->MaxParticles, 0, 0);

			cmdList->EndRendering();
		}
	};
}