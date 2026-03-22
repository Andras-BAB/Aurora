#include "aupch.h"
#include "DirectX12Framebuffer.h"

namespace Aurora {
    DirectX12Framebuffer::DirectX12Framebuffer(const FramebufferSpecification& spec) {
    }

    void DirectX12Framebuffer::Bind() {
    }

    void DirectX12Framebuffer::Unbind() {
    }

    void DirectX12Framebuffer::Resize(uint32_t width, uint32_t height) {
    }

    void DirectX12Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value) {
    }

    uint32_t DirectX12Framebuffer::GetColorAttachmentRendererID(uint32_t index) const {
        return 0;
    }

    const FramebufferSpecification& DirectX12Framebuffer::GetSpecification() const {
        return m_Specification;
    }

    void DirectX12Framebuffer::Invalidate() {
    }
}
