#include "aupch.h"
#include "RenderPassLibrary.h"

#include "Aurora/Core/Assert.h"

namespace Aurora {

	RenderPassLibrary& RenderPassLibrary::Get() {
		static RenderPassLibrary instance;
		return instance;
	}

	void RenderPassLibrary::Add(const std::shared_ptr<RenderPass>& renderPass) {
		auto& name = renderPass->GetSpecification().name;
		Add(name, renderPass);
	}

	void RenderPassLibrary::Add(const std::string& name, const std::shared_ptr<RenderPass>& renderPass) {
		AU_CORE_ASSERT(!Exists(name), "RenderPass already exists!");
		m_RenderPasses[name] = renderPass;
	}

	std::shared_ptr<RenderPass> RenderPassLibrary::Load(const std::string& name, const RenderPassSpecification& spec) {
		auto renderPass = RenderPass::Create(spec);
		Add(name, renderPass);
		return renderPass;
	}

	std::shared_ptr<RenderPass> RenderPassLibrary::Get(const std::string& name) {
		AU_CORE_ASSERT(Exists(name), "RenderPass not found!");
		return m_RenderPasses[name];
	}

	bool RenderPassLibrary::Exists(const std::string& name) const {
		return m_RenderPasses.find(name) != m_RenderPasses.end();
	}

	std::shared_ptr<RenderPass> RenderPassLibrary::GetForwardPass() {
		if (!Exists("ForwardPass")) {
			RenderPassSpecification spec("ForwardPass");
			Load("ForwardPass", spec);
		}
		return Get("ForwardPass");
	}

	std::shared_ptr<RenderPass> RenderPassLibrary::GetShadowPass() {
		if (!Exists("ShadowPass")) {
			RenderPassSpecification spec("ShadowPass");
			spec.attachments.clear();

			// only depth attachment
			RenderPassAttachmentDesc depthAttachment;
			depthAttachment.name = "Depth";
			depthAttachment.format = FramebufferTextureFormat::DEPTH24STENCIL8;
			depthAttachment.loadOp = AttachmentLoadOp::Clear;
			depthAttachment.storeOp = AttachmentStoreOp::Store;
			depthAttachment.initialLayout = ImageLayout::Undefined;
			depthAttachment.finalLayout = ImageLayout::DepthStencilAttachment;
			depthAttachment.isDepthStencil = true;
			spec.attachments.push_back(depthAttachment);

			SubpassInfo subpass;
			subpass.depthStencilAttachment = 0;
			spec.subpasses.clear();
			spec.subpasses.push_back(subpass);

			Load("ShadowPass", spec);
		}
		return Get("ShadowPass");
	}

	std::shared_ptr<RenderPass> RenderPassLibrary::GetPostProcessPass() {
		if (!Exists("PostProcessPass")) {
			RenderPassSpecification spec("PostProcessPass");
			Load("PostProcessPass", spec);
		}
		return Get("PostProcessPass");
	}

}
