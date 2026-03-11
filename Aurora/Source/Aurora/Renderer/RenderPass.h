#pragma once

#include "IFramebuffer.h"

#include <memory>
#include <vector>
#include <string>

namespace Aurora {

	enum class AttachmentLoadOp {
		Load,
		Clear, 
		DontCare
	};

	enum class AttachmentStoreOp {
		Store,
		DontCare
	};

	enum class ImageLayout {
		Undefined,
		ColorAttachment,
		DepthStencilAttachment,
		ShaderReadOnly,
		Present,
		General
	};

	struct RenderPassAttachmentDesc {
		std::string name;
		FramebufferTextureFormat format = FramebufferTextureFormat::RGBA8;
		AttachmentLoadOp loadOp = AttachmentLoadOp::Clear;
		AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
		ImageLayout initialLayout = ImageLayout::Undefined;
		ImageLayout finalLayout = ImageLayout::ColorAttachment;
		bool isDepthStencil = false;
	};

	struct SubpassInfo {
		std::vector<uint32_t> colorAttachments;
		uint32_t depthStencilAttachment = UINT32_MAX; // UINT32_MAX = nincs depth
		std::vector<uint32_t> inputAttachments;
	};

	struct RenderPassSpecification {
		std::string name;
		std::vector<RenderPassAttachmentDesc> attachments;
		std::vector<SubpassInfo> subpasses;

		// Default egyszerű forward rendering
		RenderPassSpecification(const std::string& passName = "Default") : name(passName) {
			// Default color attachment
			RenderPassAttachmentDesc colorAttachment;
			colorAttachment.name = "Color";
			colorAttachment.format = FramebufferTextureFormat::RGBA8;
			colorAttachment.loadOp = AttachmentLoadOp::Clear;
			colorAttachment.storeOp = AttachmentStoreOp::Store;
			colorAttachment.initialLayout = ImageLayout::Undefined;
			colorAttachment.finalLayout = ImageLayout::Present;
			attachments.push_back(colorAttachment);

			// Default subpass
			SubpassInfo subpass;
			subpass.colorAttachments.push_back(0);
			subpasses.push_back(subpass);
		}
	};

	class RenderPass {
	public:
		virtual ~RenderPass() = default;

		virtual void Begin(const std::shared_ptr<class IFramebuffer>& framebuffer) = 0;
		virtual void End() = 0;

		virtual const RenderPassSpecification& GetSpecification() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static std::shared_ptr<RenderPass> Create(const RenderPassSpecification& spec);
	};

}
