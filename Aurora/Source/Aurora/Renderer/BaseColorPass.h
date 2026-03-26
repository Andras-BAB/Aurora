#pragma once

#include "IRenderPass.h"
#include "RenderCommand.h"

namespace Aurora {
	class BaseColorPass : public IRenderPass {
	public:
		GraphResourceID ColorTargetID = INVALID_RESOURCE_ID;
		GraphResourceID DepthTargetID = INVALID_RESOURCE_ID;

		uint32_t Width;
		uint32_t Height;

		BaseColorPass(GraphResourceID depthTarget, uint32_t width, uint32_t height)
			: DepthTargetID(depthTarget), Width(width), Height(height) {
		}

		void Setup(IRenderGraphBuilder& builder) override {
			GraphTextureDesc colorDesc{};
			colorDesc.Width = Width;
			colorDesc.Height = Height;
			colorDesc.Format = ImageFormat::RGBA8;
			colorDesc.Name = "SceneColor";

			ColorTargetID = builder.CreateTexture(colorDesc);
			builder.WriteRenderTarget(ColorTargetID);

			// Depth is imported, so we just mark as a write target
			if (DepthTargetID != INVALID_RESOURCE_ID) builder.WriteDepthStencil(DepthTargetID);
		}

		void Execute(IRenderCommandList* cmdList, const IRenderGraphResources& resources, const SceneData& sceneData) override {
			RenderPassInfo passInfo{};

			if (ColorTargetID != INVALID_RESOURCE_ID) {
				passInfo.ColorAttachments.push_back({ ColorTargetID });
			}
			if (DepthTargetID != INVALID_RESOURCE_ID) {
				passInfo.DepthAttachment = { .ResourceId = DepthTargetID };
				passInfo.HasDepth = true;
			}

			// setting the render target, instead of directly calling OMSetRednerTargets
			cmdList->BeginRendering(passInfo);

			RenderCommand::DrawQueue(RenderQueue::Opaque, sceneData.MainView);

			cmdList->EndRendering();
		}
	};
}
