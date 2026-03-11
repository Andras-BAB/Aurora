#pragma once
#include "Aurora/Renderer/IFramebuffer.h"
#include "Platform/DirectX/Utils/MSUtils.h"

namespace Aurora {
    
    class DirectX12Framebuffer : public IFramebuffer {
    public:
        DirectX12Framebuffer(const FramebufferSpecification& spec);

        void Bind() override;
        void Unbind() override;

        void Resize(uint32_t width, uint32_t height) override;

        void ClearAttachment(uint32_t attachmentIndex, int value) override;

        uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

        const FramebufferSpecification& GetSpecification() const override;

    private:
        MS::ComPtr<ID3D12Resource> m_FrameResource;
        FramebufferSpecification m_Specification;

        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };
    
}
